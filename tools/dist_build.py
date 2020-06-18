'''
This module configures waf to provide a command 'pack', which creates a binary
distribution package.
'''

from waflib.Build import BuildContext
from waflib import Options, Context

import tarfile
import pathlib

class Package(BuildContext):
    cmd = 'build_package'

    def execute(self):
        self._prepare()
        artifact_name = self._get_artifact_name()

        rel = BuildContext()
        rel.variant = 'release' 
        rel.execute()

        deb = BuildContext()
        deb.variant = 'debug'
        deb.execute()

        rel_artifact = rel.path.find_resource(artifact_name)
        deb_artifact = deb.path.find_resource(artifact_name)

        dist_name = 'fundament-%s-%s.tar.gz' % (self.env.DEST_OS, 
            Context.g_module.VERSION)

        dist_path = '%s/%s' % (self.out_dir, dist_name)

        p = pathlib.Path(dist_path) 
        if p.exists():
            p.unlink()

        rel_target = 'bin/%s/release/%s' % (self.env.DEST_OS, artifact_name)
        deb_target = 'bin/%s/debug/%s' % (self.env.DEST_OS, artifact_name)
        tar = tarfile.open(name=dist_path, mode='x:gz')
        tar.add(str(rel_artifact), arcname=rel_target)
        tar.add(str(deb_artifact), arcname=deb_target)  
        tar.add('lib/c/public/', arcname='public')
        tar.close()

    def _prepare(self):
        self.restore()
        
        if not self.all_envs:
            self.load_envs()

        self.recurse([self.run_dir])
        self.pre_build()

    def _get_artifact_name(self):
        return self.env.cshlib_PATTERN % 'fundament'

