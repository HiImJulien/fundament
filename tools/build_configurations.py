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
        debug_flags = ['/Zi', '/DDEBUG', '/D_DEBUG', '/MDd', '/Od', '/WX']
        release_flags = ['/O2', '/Oi', '/DNDEBUG', '/Gy']
    else:
        debug_flags = ['-ggdb']
        release_flags = ['-O3']

    original_env = ctx.env.derive()
    ctx.setenv('debug', env=ctx.env.derive())
    ctx.env.CFLAGS.extend(debug_flags)

    if ctx.env.CC_NAME == 'msvc':
        ctx.env.LINKFLAGS.append('/DEBUG')

    ctx.setenv('release', env=original_env)

    if '/Od' in ctx.env.CFLAGS:
        ctx.env.CFLAGS.remove('/Od')

    ctx.env.CFLAGS.extend(release_flags)

class DebugBuild(BuildContext):
    """builds fundament with debug configurations."""
    cmd = 'debug'
    variant = 'debug' 

class ReleaseBuild(BuildContext):
    """builds fundament with release configurations."""
    cmd = 'release'
    variant = 'release'

