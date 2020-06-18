'''
This module configures waf to provide build configurations such as 'debug'
and 'release'.
'''

from waflib.Build import BuildContext
from waflib.Configure import ConfigurationContext
from waflib.Errors import ConfigurationError

def configure(ctx: ConfigurationContext):
    if not hasattr(ctx.options, 'check_c_compiler'):
        raise ConfigurationError('Add configure(ctx): ctx.load(\'compiler_c\')')

    ctx.setenv('debug', env=ctx.env.derive())
    ctx.env.CFLAGS = ['-ggdb']
    ctx.define('DEBUG', 1)

    ctx.setenv('release', env=ctx.env.derive())
    ctx.env.CFLAGS = ['-O3']
    ctx.define('RELEASE', 1) 

class BuildDebug(BuildContext):
    cmd = 'build'
    variant = 'debug'

class BuildRelease(BuildContext):
    cmd = 'build_release'
    variant = 'release'
