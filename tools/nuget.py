"""
This module configures waf to create and push a nuget package.
"""

from waflib import Task
from waflib.Build import BuildContext

def configure(ctx):
    ctx.find_program('NuGet', mandatory=False)

class NuGetPack(Task.Task):
    color = 'PINK'

    def run(self):
        self.bld.path.make_node('nuget')

        return self.exec_command(
            '${NuGet} pack'
        )

class NuGetPush(Task.Task):
    pass

class NuGet(BuildContext):
    cmd = 'nuget'

    def execute(self):
        self._prepare()

        release_build = BuildContext()
        release_build.variant = 'release'
        release_build.execute()

        debug_build = BuildContext()
        debug_build.variant = 'debug'
        debug_build.execute()

         

    def _prepare(self):
        self.restore()
        
        if not self.all_envs:
            self.load_envs()

        self.recurse([self.run_dir])
        self.pre_build()
