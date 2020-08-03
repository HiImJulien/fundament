"""This module extends waf with a custom project format. Additionally, it 
introduces sensible default configurations, such as debug and or release."""

from dataclasses import dataclass
from typing import List, Any

import os
import xml.etree.ElementTree as ET

from waflib.Build import BuildContext
from waflib.Configure import conf
from waflib.Node import Node
from waflib.Task import Task

from waflib import Context, TaskGen


@dataclass
class Meta:
    uid: str
    version: str
    description: str
    tags: List[str]
    authors: List[str]
    owners: List[str]
    project_url: str
    logo: str


@dataclass
class Dependency:
    type: str
    name: str


@dataclass
class Product:
    type: str
    name: str
    files: List[str]
    includes: List[str]


@dataclass
class Package:
    type: str
    repository: str


@dataclass
class Module:
    meta: Meta
    dependencies: List[Dependency]
    product: Product
    packages: List[Package]
    source: Node

    @staticmethod
    def from_node(ctx, node):
        module = node.read_json()

        meta_dict = module.get('meta', {})
        meta = Meta(meta_dict.get('uid', ''), meta_dict.get('version', ''),
                    meta_dict.get('description', ''),
                    meta_dict.get('tags', []), meta_dict.get('authors', []),
                    meta_dict.get('owners', []),
                    meta_dict.get('project_url', ''),
                    meta_dict.get('logo', ''))

        dependencies = []
        dependencies_dict = module.get('dependencies', [])
        for dependency in dependencies_dict:
            if dependency.get('target') != ctx.env.DEST_OS:
                continue

            dependencies.append(
                Dependency(dependency.get('type', ''),
                           dependency.get('name', '')))

        product_dict = module.get('product', {})
        product = Product(
            product_dict.get('type', ''), product_dict.get('name', ''),
            product_dict.get('files', []) +
            product_dict.get(f'{ctx.env.DEST_OS}:files', []),
            product_dict.get('includes', []))

        package_dict = module.get('package', [])
        packages = []
        for package in package_dict:
            packages.append(
                Package(package.get('type', ''), package.get('repository',
                                                             '')))

        return Module(meta, dependencies, product, packages, node)


def init(ctx: Context):
    BuildContext.variant = 'debug'


def options(ctx: Context):
    init(ctx)
    ctx.load('compiler_c')


def configure(ctx: Context):
    ctx.load('compiler_c')
    ctx.add_os_flags('CI')

    git_path = ctx.find_program('git', mandatory=False)
    Context.g_module.VERSION = '0.0.0'

    if git_path:
        cmd = [str(git_path[0]), 'describe', '--tags', '--abbrev=0']
        latest_tag = ctx.cmd_and_log(cmd=cmd)

        if latest_tag.endswith("\n"):
            latest_tag = latest_tag[:-1]

        Context.g_module.VERSION = latest_tag

        cmd = [str(git_path[0]), "rev-parse", "--abbrev-ref", "HEAD"]
        branch = ctx.cmd_and_log(cmd)

        if branch.endswith("\n"):
            branch = branch[:-1]

        ctx.env.IS_PRODUCTION = (branch == "master")
    else:
        ctx.env.IS_PRODUCTION = False

    ctx.env.VERSION = Context.g_module.VERSION
    ctx.env.BUILD_NUMBER = os.environ.get('GITHUB_RUN_NUMBER', 0)

    ctx.find_program('nuget', mandatory=False, var="NUGET")


def check_dependencies(ctx: Context, dependencies: List[Dependency]):
    for dependency in dependencies:
        if dependency.type == 'lib':
            ctx.check(lib=dependency.name, uselib_store=dependency.name)

        if dependency.type == 'pkg':
            ctx.check_cfg(package=dependency.name,
                          uselib_store=dependency.name,
                          args=['--cflags', '--libs'])

        if dependency.type == 'framework':
            ctx.check(framework=dependency.name,
                      uselib_store=dependency.name,
                      msg=f'Checking for framework {dependency.name}')

        if dependency.type == 'header':
            ctx.check(header='linux/input-event-codes.h',
                      msg=f'Checking for header {dependency.name}')


def build_product(ctx: Context, dir: Node, product: Product,
                  dependencies: List[Dependency]):
    proc = None

    defines = []
    if product.type == 'shared':
        proc = ctx.shlib
        defines.append('FUNDAMENT_EXPORTS')
    elif product.type == 'static':
        proc = ctx.stlib
    elif product.type == 'application':
        proc = ctx.program

    includes = [dir.find_dir(incl) for incl in product.includes]
    sources = [dir.find_resource(file) for file in product.files]

    proc(target=product.name,
         includes=includes,
         source=sources,
         defines=defines,
         use=[dep.name for dep in dependencies])


@conf
def module(ctx: Context, dir_path: str):
    """
    Reads the module.json from given directory.
    """
    directory = ctx.path.find_dir(dir_path)
    module_path = directory.find_resource('module.json')

    if not module_path:
        ctx.fatal(f"No such file 'module.json' in {directory}.")

    module = Module.from_node(ctx, module_path)

    if ctx.cmd == 'configure':
        check_dependencies(ctx, module.dependencies)
    elif ctx.cmd == 'build' or ctx.cmd == 'release' or ctx.cmd == 'debug':
        build_product(ctx, directory, module.product, module.dependencies)
    elif ctx.cmd == 'package':
        if module.packages:
            ctx.modules.append(module)


@conf
def create_variant_configurations(ctx: Context):
    debug = ctx.env.derive()
    debug.detach()

    release = ctx.env.derive()
    release.detach()

    if ctx.env.CC_NAME == 'msvc':
        debug.CFLAGS.extend(
            ['/Zi', '/DDEBUG', '/D_DEBUG', '/MDd', '/Od', '/WX', '/FS'])
        release.CFLAGS.extend(['/O2', '/Oi', '/DNDEBUG', '/Gy', '/FS'])
    else:
        debug.CFLAGS.append('-ggdb')
        release.CFLAGS.append('-O3')

    ctx.setenv('debug', debug)
    ctx.setenv('release', release)


class GenerateNuspecFileTask(Task):

    def __init__(self, module: Module, env):
        super().__init__(env=env)
        self.module = module

    def keyword(self):
        return "Generating nuspec from"

    def set_meta(self, meta: Meta):
        self.meta = meta

    def run(self):
        meta = getattr(self, "meta", None)
        if not meta:
            raise AttributeError("No meta set.")

        root = ET.Element('package')
        xmeta = ET.SubElement(root, "metadata")
        ET.SubElement(xmeta, "id").text = meta.uid
        version = ET.SubElement(xmeta, "version")
        
        if self.bld.env.IS_PRODUCTION:
            version.text = f"{self.bld.env.VERSION}.{self.bld.env.BUILD_NUMBER}"
        else:
            version.text = f"{self.bld.env.VERSION}.{self.bld.env.BUILD_NUMBER}" + "-dev" 

        ET.SubElement(xmeta, "description").text = meta.description
        ET.SubElement(xmeta, "tags").text = ", ".join(meta.tags)
        ET.SubElement(xmeta, "authors").text = ", ".join(meta.authors)
        ET.SubElement(xmeta, "projectUrl").text = meta.project_url
        ET.SubElement(xmeta,
                      "repository",
                      type="git",
                      url="https://github.com/HiImJulien/fundament.git")

        files = ET.SubElement(root, "files")
        ET.SubElement(files,
                      "file",
                      src="**\\*.dll",
                      target="build\\native\\bin")
        ET.SubElement(files,
                      "file",
                      src="**\\*.manifest",
                      target="build\\native\\bin")
        ET.SubElement(files,
                      "file",
                      src="**\\*.lib",
                      target="build\\native\\lib")
        ET.SubElement(files,
                      "file",
                      src="..\\lib\\c\\public\\fundament\\*.h",
                      target="build\\native\\include\\fundament")
        ET.SubElement(files,
                      "file",
                      src=f"{meta.uid}.targets",
                      target=f"build\\native\\{meta.uid}.targets")


        if self.module.meta.logo:
            logo_node = self.module.source.parent.find_resource(self.module.meta.logo)
            
            ET.SubElement(files,
                "file",
                src=str(logo_node),
                target="images\\logo.png")

            ET.SubElement(xmeta, "icon").text = "images\\logo.png"

        tree = ET.ElementTree(root)
        tree.write(str(self.outputs[0]))


class GenerateTargetsFileTask(Task):

    def __init__(self, module: module, env):
        super().__init__(env=env)
        self.module = module

    def run(self):
        project = ET.Element(
            "Project",
            ToolsVersion="4.0",
            xmlns="http://schemas.microsoft.com/developer/msbuild/2003")
        item_def_group = ET.SubElement(project, "ItemDefinitionGroup")

        compile_group = ET.SubElement(item_def_group, "ClCompile")
        include_directories = ET.SubElement(compile_group, "AdditionalIncludeDirectories")
        include_directories.text = "$(MSBuildThisFileDirectory)include\\;%(AdditionalIncludeDirectories)"

        link_group = ET.SubElement(item_def_group, "Link")
        link_lib = ET.SubElement(link_group, "AdditionalDependencies")
        link_lib.text = f"$(MSBuildThisFileDirectory)lib\\$(Configuration.ToLower())\\{self.module.product.name}.lib;%(AdditionalDependencies)"

        escape_uid = self.module.meta.uid.replace('.', '_')

        item_group = ET.SubElement(project, "ItemGroup")
        ET.SubElement(
            item_group,
            f"{escape_uid}Files",
            Include=
            f"$(MSBuildThisFileDirectory)bin\\$(Configuration.ToLower())\\{self.module.product.name}.dll"
        )
        ET.SubElement(
            item_group,
            f"{escape_uid}Files",
            Include=
            f"$(MSBuildThisFileDirectory)bin\\$(Configuration.ToLower())\\{self.module.product.name}.dll.manifest"
        )

        copy_target = ET.SubElement(project,
            "Target",
            Name=f"{escape_uid}CopyFiles",
            AfterTargets="AfterBuild"
        )

        ET.SubElement(
            copy_target,
            "Copy",
            SourceFiles=f"@({escape_uid}Files)",
            DestinationFiles=
            f"@({escape_uid}Files->'$(TargetDir)%(RecursiveDir)%(Filename)%(Extension)')",
            SkipUnchangedFiles="true"
        )

        tree = ET.ElementTree(project)
        tree.write(str(self.outputs[0]))


class PackNugetTask(Task):
    def run(self):
        self.exec_command(f'nuget pack {self.inputs[0]}', cwd=self.bld.out_dir)


class PackageContext(BuildContext):
    """Create a NuGet package."""
    cmd = 'package'

    def __init__(self):
        super().__init__()

        self.modules = []

    def execute(self):
        self.restore()

        if not self.all_envs:
            self.load_envs()

        self.recurse([self.run_dir])
        self.pre_build()

        rel = BuildContext()
        rel.variant = 'release'
        rel.execute()

        deb = BuildContext()
        deb.variant = 'debug'
        deb.execute()

        if self.env.DEST_OS != 'win32':
            return

        for mod in self.modules:
            gen = GenerateNuspecFileTask(module=mod, env=self.env)
            gen.set_meta(mod.meta)
            gen.set_inputs(mod.source)

            nuspec_file = self.path.find_resource(f'../{mod.meta.uid}.nuspec')
            if nuspec_file and nuspec_file.exists():
                nuspec_file.delete()

            nuspec_file = self.path.find_or_declare(f'../{mod.meta.uid}.nuspec')

            gen.set_outputs(nuspec_file)
            self.add_to_group(gen)

            res = self.path.find_or_declare(f'../{mod.meta.uid}.targets')
            gen_targets = GenerateTargetsFileTask(module=mod, env=self.env)
            gen_targets.set_inputs(mod.source)
            gen_targets.set_outputs(res)
            self.add_to_group(gen_targets)

            nuget = None
            if self.env.IS_PRODUCTION:
                nuget = self.path.find_or_declare(f'../{mod.meta.uid}.{self.env.VERSION}.{self.env.BUILD_NUMBER}.nupkg')
            else:
                nuget = self.path.find_or_declare(f'../{mod.meta.uid}.{self.env.VERSION}.{self.env.BUILD_NUMBER}-dev.nupkg')

            pack = PackNugetTask(env=self.env)
            pack.set_inputs(gen.outputs[0])
            pack.set_outputs(nuget)
            pack.set_run_after(gen)
            pack.set_run_after(gen_targets)
            self.add_to_group(pack)

        self.execute_build()


@TaskGen.extension('.m')
def objc_hook(self, node):
    return self.create_compiled_task('m', node)


class m(Task):
    run_str = '${CC} ${ARCH_ST:ARCH} ${MMFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${CC_SRC_F}${SRC} ${CC_TGT_F}${TGT}'
    ext_in = ['.h']
