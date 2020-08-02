"""This module extends waf with a custom project format. Additionally, it 
introduces sensible default configurations, such as debug and or release."""

from dataclasses import dataclass
from typing import List, Any

import xml.etree.ElementTree as ET

from waflib.Build import BuildContext
from waflib.Configure import conf
from waflib.Node import Node
from waflib.Task import Task

from waflib import Context

@dataclass
class Meta:
	uid: str
	version: str
	description: str
	tags: List[str]
	authors: List[str]
	owners: List[str]
	project_url: str

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
		meta = Meta(
			meta_dict.get('uid', ''),
			meta_dict.get('version', ''),
			meta_dict.get('description', ''),
			meta_dict.get('tags', []),
			meta_dict.get('authors', []),
			meta_dict.get('owners', []),
			meta_dict.get('project_url', '')
		)

		dependencies = []
		dependencies_dict = module.get('dependencies', [])
		for dependency in dependencies_dict:
			if dependency.get('target') != ctx.env.DEST_OS:
				continue

			dependencies.append(Dependency(
				dependency.get('type', ''),
				dependency.get('name', '')
			))

		product_dict = module.get('product', {})
		product = Product(
			product_dict.get('type', ''),
			product_dict.get('name', ''),
			product_dict.get('files', [])
			+ product_dict.get(f'{ctx.env.DEST_OS}:files', []),
			product_dict.get('includes', [])
		)

		package_dict = module.get('package', [])
		packages = []
		for package in package_dict:
			packages.append(Package(
			package.get('type', ''),
			package.get('repository', '')
		))

		return Module(
			meta,
			dependencies,
			product,
			packages,
			node
		)


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
		latest_tag = ctx.cmd_and_log([git_path, 'describe', '--tags', '--abbrev=0'])

		if latest_tag.endswith("\n"):
			latest_tag = latest_tag[:-1]

		Context.g_module.VERSION = latest_tag

	ctx.env.VERSION = Context.g_module.VERSION
	ctx.add_os_flags('CI')
	if ctx.env.CI:
		ctx.add_os_flags('GITHUB_RUN_NUMBER')
		# Context.g_module.VERSION += f"-{ctx.env.GITHUB_RUN_NUMBER}"

	ctx.find_program('nuget', mandatory=False, var="NUGET")	


def check_dependencies(ctx: Context, dependencies: List[Dependency]):
	for dependency in dependencies:
		if dependency.type == 'lib':
			ctx.check(lib=dependency.name, uselib_store=dependency.name)

		if dependency.type == 'pkg':
			ctx.check_cfg(
				package=dependency.name,
				uselib_store=dependency.name,
				args=['--cflags', '--libs']
			)

		if dependency.type == 'framework':
			ctx.check(framework=dependency.name, uselib_store=dependency.name)

		if dependency.type == 'header':
			ctx.check(header='linux/input-event-codes.h')

def build_product(ctx: Context, dir: Node, product: Product, dependencies: List[Dependency]):
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

	proc(
		target=product.name,
		includes=includes,
		source=sources,
		defines=defines,
		use=[dep.name for dep in dependencies]
	)

def build_nuget_package(ctx: Context, product: Product, meta: Meta):
	if ctx.cmd != 'package':
		return

	if not ctx.env.NUGET:
		ctx.to_log("No NuGet client was found in PATH.\n")

	nuspec = ctx.path.find_or_declare(f'../{product.name}.nuspec')

	root = ET.Element('package')
	xmeta = ET.SubElement(root, "meta")
	ET.SubElement(xmeta, "id").text = meta.uid
	ET.SubElement(xmeta, "version").text = ctx.env.VERSION
	ET.SubElement(xmeta, "description").text = meta.description
	ET.SubElement(xmeta, "tags").text = ", ".join(meta.tags)
	ET.SubElement(xmeta, "authors").text = ", ".join(meta.authors)
	ET.SubElement(xmeta, "projectUrl").text = meta.project_url

	files = ET.SubElement(root, "files")
	ET.SubElement(files, "file", src="**\\*.dll", target="native/bin")
	ET.SubElement(files, "file", src="**\\*.exp", target="native/bin")
	ET.SubElement(files, "file", src="**\\*.manifest", target="native/bin")
	ET.SubElement(files, "file", src="**\\*.lib", target="native/lib")
	ET.SubElement(files, "file", src="..\\..\\lib\\c\\public\\fundament\\*.h", target="native\\include\\fundament")

	tree = ET.ElementTree(root)
	path = str(nuspec)
	tree.write(path)

	ctx.to_log("Ensuring debug and release build.")
	release = BuildContext()
	release.variant = 'release'
	release.execute()

	debug = BuildContext()
	debug.variant = 'debug'
	debug.execute()

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
		debug.CFLAGS.extend(['/Zi', '/DDEBUG', '/D_DEBUG', '/MDd', '/Od', '/WX', '/FS'])
		release.CFLAGS.extend(['/O2', '/Oi', '/DNDEBUG', '/Gy', '/FS'])
	else:
		debug.CFLAGS.append('-ggdb')
		release.CFLAGS.append('-O3')

	ctx.setenv('debug', debug)
	ctx.setenv('release', release)


class GenerateNuspecFile(Task):

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
		ET.SubElement(xmeta, "version").text = self.bld.env.VERSION
		ET.SubElement(xmeta, "description").text = meta.description
		ET.SubElement(xmeta, "tags").text = ", ".join(meta.tags)
		ET.SubElement(xmeta, "authors").text = ", ".join(meta.authors)
		ET.SubElement(xmeta, "projectUrl").text = meta.project_url

		files = ET.SubElement(root, "files")
		ET.SubElement(files, "file", src="**\\*.dll", target="native/bin")
		ET.SubElement(files, "file", src="**\\*.manifest", target="native/bin")
		ET.SubElement(files, "file", src="**\\*.lib", target="native/lib")
		ET.SubElement(files, "file", src="..\\lib\\c\\public\\fundament\\*.h", target="native\\include\\fundament")

		tree = ET.ElementTree(root)
		tree.write(str(self.outputs[0]))

class PackNuget(Task):

	def run(self):
		self.exec_command(
			f'nuget pack {self.inputs[0]} -OutputFileNamesWithoutVersion',
			cwd = self.bld.out_dir
		)

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

		for mod in self.modules:
			gen = GenerateNuspecFile(env=self.env)
			gen.set_meta(mod.meta)
			gen.set_inputs(mod.source)

			res = self.path.find_or_declare(f'../{mod.product.name}.nuspec')
			gen.set_outputs(res)
			self.add_to_group(gen)

			pack = PackNuget(env=self.env)
			pack.set_inputs(gen.outputs[0])
			pack.set_outputs(self.path.find_or_declare(f'../{mod.meta.uid}.nupkg'))
			pack.set_run_after(gen)
			self.add_to_group(pack)

		self.execute_build()