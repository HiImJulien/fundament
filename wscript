'''
This file is used by the Waf build system to drive this project's build.
'''

import enum
import tarfile
import os
import pathlib

from waflib import Options
from waflib.Context import Context
from waflib.Build import BuildContext, CleanContext, InstallContext, \
                         UninstallContext
from waflib.Configure import ConfigurationContext
from waflib.Options import OptionsContext

APPNAME = 'fundament'
VERSION = '0.2'
GROUP = 'me.juliankirsch'

RC_DATA = {
    'FILEVERSION' : '0,0,0,0',
    'PRODUCTVERSION' : VERSION,
    'COMPANYNAME' : 'juliankirsch.me',
    'FILEDESCRIPTION' : 'A slick foundation for game developers.',
    'COPYRIGHT' : 'Copyright 2020 Julian Kirsch <contact@juliankirsch.me>',
    'TRADEMARK' : 'All Rights Reserved',
    'ORIGINALFILENAME' : 'fundament.dll',
    'PRODUCTNAME' : 'fundament Runtime',
}

def init(ctx: Context):
    # The previous way to ensure, that all builds were either debug or release
    # deemed a bit hacky.
    # This is the preferred way, at it is proposed by the waf dev.
    for cls in [BuildContext, CleanContext, InstallContext, UninstallContext]:
        cls.variant = 'debug'

    if not os.environ.get('CI'):
        return

    major, minor = VERSION.split('.')
    build_number = os.environ.get('GITHUB_RUN_NUMBER')
    RC_DATA['PRODUCTVERSION'] = f'{major}.{minor}-{build_number}' 

    file_version = VERSION.replace('.', ',')
    file_version = f'{file_version}.{build_number}.0'
    RC_DATA['FILEVERSION'] = file_version

def options(ctx: OptionsContext):
    ctx.load('compiler_c')
    ctx.load('build_configurations', tooldir='tools')
    ctx.load('dist_build', tooldir='tools')
    ctx.load('objc', tooldir='tools')

    ctx.load('clang_compilation_database', tooldir='tools')

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

        ctx.check(header='linux/input-event-codes.h', msg='Checking for header input-event-codes.h')

    if ctx.env.DEST_OS == 'darwin':
        ctx.check(framework='AppKit', msg='Checking for framework Appkit', 
                    uselib_store='AppKit')

        ctx.load('objc', tooldir='tools')

    if ctx.env.DEST_OS == 'win32':
        ctx.check(lib='user32', msg='Checking for library user32',
                    uselib_store='user32')

    if ctx.env.CC_NAME == 'msvc':
        ctx.env.CFLAGS.extend(['/FS', '/DFUNDAMENT_EXPORTS'])

    # Remember whether to build the sample or not.
    ctx.env.with_samples = ctx.options.with_samples

    # Branch the configuration into a debug and release
    # variant.
    ctx.load('build_configurations', tooldir='tools')
    ctx.load('dist_build', tooldir='tools')
    ctx.load('clang_compilation_database', tooldir='tools')

    ctx.define('FUNDAMENT_VERSION', VERSION)
    ctx.define('FUNDAMENT_VERSION_MAJOR', int(VERSION.split('.')[0]))
    ctx.define('FUNDAMENT_VERSION_MINOR', int(VERSION.split('.')[1]))
    ctx.write_config_header('config.h')

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
            'lib/c/private/xcb/input_key_map_xcb.c'
        ])

        dependencies.extend([
            'xcb',
            'xcb-xinput',
            'x11-xcb'
        ])

    if ctx.env.DEST_OS == 'darwin':
        source.extend([
            'lib/c/private/appkit/window_appkit.m',
            'lib/c/private/appkit/input_appkit.m',
            'lib/c/private/appkit/input_key_map_appkit.c'
        ])

        dependencies.extend([
            'AppKit'
        ])

    if ctx.env.DEST_OS == 'win32':
        ctx(
            features='subst',
            source='lib/c/private/win32/fundament.rc.in',
            target='fundament.rc',
            **RC_DATA
        )

        source.extend([
            'lib/c/private/win32/window_win32.c',
            'lib/c/private/win32/input_key_map_win32.c',
            'fundament.rc'
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

    if ctx.env.with_samples:
        ctx.program(
            target='simple_window',
            source='samples/c/simple_window.c',
            use='fundament',
            rpath='$ORIGIN'
        )

