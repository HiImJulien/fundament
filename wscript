'''
This file is used by the Waf build system to drive this project's build.
'''

import enum

from waflib import Options
from waflib.Context import Context
from waflib.Build import BuildContext
from waflib.Configure import ConfigurationContext
from waflib.Options import OptionsContext

APPNAME = 'fundament'
VERSION = '0.2'

def options(ctx: OptionsContext):
    ctx.load('compiler_c')

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
            'lib/c/private/window_Xcb.c',
            'lib/c/private/input_Xcb.c'
        ])

        dependencies.extend([
            'xcb',
            'xcb-xinput',
            'x11-xcb'
        ])

    ctx.shlib(
        target='fundament',
        source=source,
        includes='lib/c/public',
        use=dependencies    
    )

    ctx.install_files('public', [
        'lib/c/public/fundament/api.h',
        'lib/c/public/fundament/config.h',
        'lib/c/public/fundament/event.h',
        'lib/c/public/fundament/input.h',
        'lib/c/public/fundament/window.h',
    ])

                


