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

def options(ctx: OptionsContext):
    ctx.load('compiler_c')
    # ctx.load('build_configurations', tooldir='tools')
    # ctx.load('dist_build', tooldir='tools')
    ctx.load('objc', tooldir='tools')

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
        ctx.check(framework='AppKit', msg='Checking for framework Appkit', uselib_store='AppKit')

        ctx.load('objc', tooldir='tools')

    # Branch the configuration into a debug and release
    # variant.
    # ctx.load('build_configurations', tooldir='tools')
    # ctx.load('dist_build', tooldir='tools')

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

    if ctx.env.DEST_OS == 'darwin':
        source.extend([
            'lib/c/private/window_Appkit.m',
            'lib/c/private/input_AppKit.m' 
        ])

        dependencies.extend([
            'AppKit'
        ])

    artefact = ctx.shlib(
        target='fundament',
        source=source,
        includes='lib/c/public/',
        use=dependencies    
    )

