'''
This module configures waf to recognize and build '.m' files.
'''

from waflib.Task import Task
from waflib.TaskGen import extension

def objc_hook(self, node):
    return self.create_compiled_task('m', node)

class m(Task):
    run_str = '${CC} ${ARCH_ST:ARCH} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ${DEFINES_ST:DEFINES} ${CXX_SRC_F}${SRC} ${CXX_TGT_F}${TGT}'

    vars = ['CCDEPS']
    ext_in = ['.h']

