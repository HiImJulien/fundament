'''
This module configures waf to provide a command 'binary_dist', creating 
a binary package.   
'''

from waflib import Build
import shutil, os, sys, platform

class BinaryDist(Build.InstallContext):
    '''
    Creates archives containing both debug and release artifacts.
    '''
    cmd = 'pack'
    fun = 'build'

    def init_dirs(self, *k, **kw):
        super().init_dirs(*k, **kw)
        self._pkgroot = self.bldnode.make_node('.dist')
        
        if os.path.exists(self._pkgroot.abspath()):
            shutil.rmtree(self._pkgroot.abspath()) 

        self._pkgroot.mkdir()
        self.options.destdir = self._pkgroot.abspath()

    def execute(self, *k, **kw):
        super().execute(*k, **kw)

