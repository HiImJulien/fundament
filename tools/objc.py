'''
This module configures waf to recognize and build '.m' files.
'''

from waflib import TaskGen, Task

@TaskGen.extension('.m')
def objc_hook(self, node):
    return self.create_compiled_task('m', node)

class m(Task.Task):
    run_str = '${CC} ${ARCH_ST:ARCH} ${MMFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${CC_SRC_F}${SRC} ${CC_TGT_F}${TGT}'
    ext_in = ['.h']

