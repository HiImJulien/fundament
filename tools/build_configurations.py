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

    debug_flags = []
    release_flags = []

    if ctx.env.CC_NAME == 'msvc':
        debug_flags = ['/Zi', '/DDEBUG', '/D_DEBUG', '/MDd', '/Od']
        release_flags = ['/O2', '/Oi', '/DNDEBUG', '/Gy']
    else:
        debug_flags = ['-ggdb']
        release_flags = ['-O3']

    original_env = ctx.env.derive()
    ctx.setenv('debug', env=ctx.env.derive())
    ctx.env.CFLAGS.extend(debug_flags)

    if ctx.env.CC_NAME == 'msvc':
        ctx.env.LINKFLAGS.append('/DEBUG')

    ctx.define('DEBUG', 1)

    ctx.setenv('release', env=original_env)
    ctx.env.CFLAGS.extend(release_flags)
    ctx.define('RELEASE', 1) 

class BuildDebug(BuildContext):
    cmd = 'build'
    variant = 'debug'

class BuildRelease(BuildContext):
    cmd = 'build_release'
    variant = 'release'
