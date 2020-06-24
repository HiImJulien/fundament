'''
This file is used by the Waf build system to drive this project's build.
'''

import enum
import tarfile
import pathlib

from waflib import Options
from waflib.Context import Context
from waflib.Build import BuildContext
from waflib.Configure import ConfigurationContext
from waflib.Options import OptionsContext

APPNAME = 'fundament'
VERSION = '0.2'
GROUP = 'me.juliankirsch'

def options(ctx: OptionsContext):
    ctx.load('compiler_c')
    ctx.load('build_configurations', tooldir='tools')
    ctx.load('dist_build', tooldir='tools')
    ctx.load('objc', tooldir='tools')

    ctx.add_option('--with-samples', action='store_true', default=False, 
                    help='If set, builds the samples as well.')

def configure(ctx: ConfigurationContext):
    ctx.load('compiler_c')

    if ctx.env.DEST_OS == 'linux':
        ctx.check_cfg(
            package='xcb',
            uselib_store='xcb',
            args=['--cflags', '--libs']
        )
    
        ctx.check_cfg(
            package='xcb-xinput',
            uselib_store='xcb',
            args=['--cflags', '--libs']
        )

        ctx.check_cfg(
            package='x11-xcb',
            uselib_store='x11-xcb',
            args=['--cflags', '--libs']
        )

    if ctx.env.DEST_OS == 'darwin':
        ctx.check(framework='AppKit', msg='Checking for framework Appkit', 
                    uselib_store='AppKit')

        ctx.load('objc', tooldir='tools')

    if ctx.env.DEST_OS == 'win32':
        ctx.check(lib='user32', msg='Checking for library user32',
                    uselib_store='user32')

    if ctx.env.CC_NAME == 'msvc':
        ctx.env.CFLAGS.extend(['/FS', '/DFUNDAMENT_EXPORTS'])

    # Branch the configuration into a debug and release
    # variant.
    ctx.load('build_configurations', tooldir='tools')
    ctx.load('dist_build', tooldir='tools')

def build(ctx: BuildContext):
    source = [
        'lib/c/private/input.c',
        'lib/c/private/input_common.c',
        'lib/c/private/window.c',
        'lib/c/private/window_common.c'   
    ] 

    dependencies = []

    if ctx.env.DEST_OS == 'linux':
        source.extend([
            'lib/c/private/xcb/window_xcb.c',
            'lib/c/private/xcb/input_xcb.c'
        ])

        dependencies.extend([
            'xcb',
            'xcb-xinput',
            'x11-xcb'
        ])

    if ctx.env.DEST_OS == 'darwin':
        source.extend([
            'lib/c/private/AppKit/window_AppKit.m',
            'lib/c/private/AppKit/input_AppKit.m'
        ])

        dependencies.extend([
            'AppKit'
        ])

    if ctx.env.DEST_OS == 'win32':
        source.extend([
            'lib/c/private/win32/window_win32.c',
            'lib/c/private/win32/input_win32.c'
        ])

        dependencies.extend([
            'user32'
        ]) 

    artefact = ctx.shlib(
        target='fundament',
        source=source,
        includes='lib/c/public/',
        export_includes='lib/c/public',
        use=dependencies    
    )

    if ctx.options.with_samples:
        ctx.program(
            target='simple_window',
            source='samples/c/simple_window.c',
            use='fundament',
            rpath='$ORIGIN'
        )

