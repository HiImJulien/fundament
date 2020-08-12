#! /usr/bin/python3
# encoding: utf-8
"""This script defines the build steps for the fundament project."""

__author__ = "Julian Kirsch"

import waflib
from waflib import Context, TaskGen, Task
from waflib.Build import BuildContext, StepContext, InstallContext, UninstallContext
from waflib.Configure import ConfigurationContext, conf
from waflib.Options import OptionsContext

#=======================================================================================================================
# The following section defines utilities.
#=======================================================================================================================

@conf
def target_is_linux(ctx):
    return (ctx.env.DEST_OS == "linux")

@conf
def target_is_macOS(ctx):
    return (ctx.env.DEST_OS == "darwin")

@conf
def target_is_win32(ctx):
    return (ctx.env.DEST_OS == "win32")

#=======================================================================================================================
# Extended waf to build Objective C sources. This is based off 
# https://www.jeffongames.com/2012/12/using-waf-to-build-an-ios-universal-framework/
#=======================================================================================================================

@TaskGen.extension('.m')
def objective_c_hook(self, node):
    return self.create_compiled_task('m', node)

class m(Task.Task):
    ext_in = ['.h']
    run_str = '${CC} ${ARCH_ST:ARCH} ${MMFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} ' \
        '${DEFINES_ST:DEFINES} ${CC_SRC_F}${SRC} ${CC_TGT_F}${TGT}'
    
#=======================================================================================================================
# The following sections defines the configuration and build step.
#=======================================================================================================================

def init(ctx: Context):
    pass

def options(ctx: OptionsContext):
    contexts = waflib.Context.classes
    contexts.remove(StepContext)
    contexts.remove(InstallContext)
    contexts.remove(UninstallContext)

    ctx.load("compiler_c")

def configure(ctx: ConfigurationContext):
    ctx.load("compiler_c")

    # Step 1: Check, whether all external dependencies exist.
    if ctx.target_is_linux():
        ctx.check_cfg(package="xcb", uselib_store="fundament_deps", args=["--cflags", "--libs"])
        ctx.check_cfg(package="xcb-xinput", uselib_store="fundament_deps", args=["--cflags", "--libs"])
        ctx.check_cfg(package="x11-xcb", uselib_store="fundament_deps", args=["--cflags", "--libs"])
        ctx.check(header="linux/input-event-codes.h", msg="Checking for 'linux/input-event-codes.h'")

    if ctx.target_is_win32():
        ctx.check(lib="user32", uselib_store="fundament_deps", msg="Checking for 'user32.lib'")

    if ctx.target_is_macOS():
        ctx.check(framework="AppKit", uselib_store="fundament_deps", msg="Checking for 'AppKit'")

    # Step 2: Create both, debug and release configurations.
    # This step must be done AFTER gathering depenendencies; allowing us to 
    # query for dependencies once and reusing them twice.

    # Detach both, since derive only create shallow copies.
    dbg = ctx.env.derive().detach()
    rel = ctx.env.derive().detach()

    if ctx.env.CC_NAME == "msvc":
        dbg.CFLAGS += ["/Zi", "/DDEBUG", "/D_DEBUG", "/MDd", "/Od", "/WX"]
        dbg.LINKFLAGS += ["/DEBUG"]
        rel.CFLAGS += ["/O2", "/Oi", "/DNDEBUG", "/Gy"]
    elif ctx.env.CC_NAME == "gcc" or ctx.env.CC_NAME == "clang":
        dbg.CFLAGS += ["-ggdb"]
        rel.CFLAGS += ["-O3"]

    ctx.setenv("release", env=rel)
    ctx.setenv("debug", env=dbg)

def build(ctx: BuildContext):    
    sources = [
        "platform/c/private/input.c",
        "platform/c/private/input_common.c",
        "platform/c/private/window.c",
        "platform/c/private/window_common.c"
    ]

    linux_sources = [
        "platform/c/private/linux/input_key_map_xcb.c",
        "platform/c/private/linux/window_xcb.c"
    ]

    macOS_sources = [
        "platform/c/private/macOS/input_key_map_appkit.c",
        "platform/c/private/macOS/window_appkit.m"
    ]

    win32_sources = [
        "platform/c/private/win32/input_key_map_win32.c",
        "platform/c/private/win32/window_win32.c"
    ]

    if ctx.target_is_linux():
        sources += linux_sources
    elif ctx.target_is_macOS():
        sources += macOS_sources
    elif ctx.target_is_win32():
        sources += win32_sources
    else:
        ctx.fatal("Trying to build for unknown target OS.")

    ctx.shlib(
        target="fundament",
        source=sources,
        includes="platform/c/public",
        defines="FUNDAMENT_EXPORTS",
        export_includes="platform/c/public",
        use="fundament_deps",
    )
    

    
