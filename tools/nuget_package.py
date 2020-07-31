"""
This module configures waf to package fundament as a NuGet package.
"""

import platform
import os

from waflib.Build import BuildContext
from waflib.Configure import conf
from waflib.Task import Task
from waflib import Context

class NugetPack(Task):

    def __str__(self):
        return "Creating NuGet package."

    def run():
        pass

def configure(ctx):
    if ctx.env.DEST_OS != 'win32':
        return
    
    ctx.find_program('nuget', var='NUGET')

class NugetPack(BuildContext):
    cmd = 'nuget_pack'

    def execute(self):
        self._prepare()

        rel = BuildContext()
        rel.variant = 'release'
        rel.execute()

        deb = BuildContext()
        deb.variant = 'debug'
        deb.execute()

        version = Context.g_module.VERSION.split('.')
        if len(version) == 2:
            version = '%s.0' % Context.g_module.VERSION

        target_file = 'build/nuget/fundament.%s.nupkg' % version
        target_file = self.path.make_node(target_file)
        nuspec_file = self.path.find_resource('tools/templates/fundament.nuspec')

        self.env.VERSION = version

        self(
            rule='nuget pack ${SRC} -Version ${VERSION} -BasePath . -OutputDirectory ../../build/nuget',
            source=nuspec_file,
            target=target_file
        )

        self.target = target_file
        
        self.execute_build()

        self.post_build()
   
    def _prepare(self):
        self.restore() 

        if not self.all_envs:
            self.load_envs()

        self.recurse([self.run_dir])
        self.pre_build()

class NugetPush(BuildContext):
    cmd = 'nuget_publish'

    def execute(self):
        bld = NugetPack()
        bld.execute()

        target_file = bld.target

        if 'NUGET_API_KEY' not in os.environ:
            raise ValueError("Env var NUGET_API_KEY not defined.")

        self.env.API_KEY = os.environ['NUGET_API_KEY']

        self(
            rule='nuget push ${SRC} ${API_KEY} -src https://nuget.pkg.github.com/HiImJulien/index.json',
            src=target_file
        ) 

        self.execute_build()
