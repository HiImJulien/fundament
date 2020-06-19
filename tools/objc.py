'''
This module configures waf to recognize and build '.m' files.
'''

from waflib import TaskGen, Task

@TaskGen.extension('.m')
def objc_hook(self, node):
    return self.create_compiled_task('m', node)

class m(Task.Task):
    run_str = '${CXX} ${ARCH_ST:ARCH} ${MMFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${CXX_SRC_F}${SRC} ${CXX_TGT_F}${TGT}'
    ext_in = ['.h']

