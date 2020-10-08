#! /usr/bin/python3
# encoding: utf-8
"""This script defines the build steps for the fundament project."""

__author__ = "Julian Kirsch"

import waflib
from waflib import Context, TaskGen, Task
from waflib.Build import BuildContext, StepContext, InstallContext, UninstallContext
from waflib.Configure import ConfigurationContext, conf
from waflib.Options import OptionsContext

#===============================================================================
# The following section defines utilities.
#===============================================================================

@conf
def target_is_linux(ctx):
    return (ctx.env.DEST_OS == "linux")

@conf
def target_is_macOS(ctx):
    return (ctx.env.DEST_OS == "darwin")

@conf
def target_is_win32(ctx):
    return (ctx.env.DEST_OS == "win32")

@conf
def cc_is_msvc(ctx):
    return (ctx.env.CC_NAME == "msvc")

@conf
def cc_is_gcc_kind(ctx):
    return (ctx.env.CC_NAME == "gcc") or (ctx.env.CCC_NAME == "clang")

#===============================================================================
# Extended waf to build Objective C sources. This is based off 
# https://www.jeffongames.com/2012/12/using-waf-to-build-an-ios-universal-framework/
#===============================================================================

@TaskGen.extension(".m")
def objective_c_hook(self, node):
    return self.create_compiled_task("m", node)

class m(Task.Task):
    ext_in = [".h"]
    run_str = "${CC} ${ARCH_ST:ARCH} ${MMFLAGS} ${FRAMEWORKPATH_ST:FRAMEWORKPATH} ${CPPPATH_ST:INCPATHS} " \
        "${DEFINES_ST:DEFINES} ${CC_SRC_F}${SRC} ${CC_TGT_F}${TGT}"
    
#===============================================================================
# The following sections defines the configuration and build step.
#===============================================================================

def init(ctx: Context):
    pass

def options(ctx: OptionsContext):
    contexts = waflib.Context.classes
    contexts.remove(StepContext)
    contexts.remove(InstallContext)
    contexts.remove(UninstallContext)

    ctx.load("compiler_c")

    ctx.add_option("--exclude-ogl", dest="exclude_ogl", default=False, action="store_true", 
        help="Whether to exclude OpenGL context abstraction. (default: False)")

    ctx.load("clang_compilation_database")

def configure(ctx: ConfigurationContext):
    ctx.load("compiler_c")

    if ctx.options.exclude_ogl:
        ctx.env.EXCLUDED_FEATURES.append_unique('ogl')

    ctx.recurse("playground")

    # Step 1: Check, whether all external dependencies exist.
    if ctx.target_is_linux():
        ctx.check_cfg(package="xcb", uselib_store="fundament_deps", args=["--cflags", "--libs"])
        ctx.check_cfg(package="xcb-xinput", uselib_store="fundament_deps", args=["--cflags", "--libs"])
        ctx.check_cfg(package="x11-xcb", uselib_store="fundament_deps", args=["--cflags", "--libs"])
        ctx.check(header="linux/input-event-codes.h", msg="Checking for 'linux/input-event-codes.h'")

    if ctx.target_is_linux() and 'ogl' not in ctx.env.EXCLUDED_FEATURES:
        ctx.check_cfg(package="glx", uselib_store="fundament_deps", args=["--cflags", "--libs"])

    if ctx.target_is_win32():
        ctx.check(lib="user32", uselib_store="fundament_deps", msg="Checking for 'user32.lib'")

    if ctx.target_is_win32() and 'ogl' not in ctx.env.EXCLUDED_FEATURES:
        ctx.check(lib="gdi32", uselib_store="fundament_deps", msg="Checking for 'gdi32.lib'")
        ctx.check(lib="opengl32", uselib_store="fundament_deps", msg="Checking for 'opengl32.lib'")

    if ctx.target_is_macOS():
        ctx.check(framework="AppKit", uselib_store="fundament_deps", msg="Checking for 'AppKit'")

    if ctx.target_is_macOS() and 'gfx' not in ctx.env.EXCLUDED_FEATURES:
        ctx.check(framework="Metal", uselib_store="fundament_deps", msg="Checking for 'Metal'")

    # Step 2: Create both, debug and release configurations.
    # This step must be done AFTER gathering depenendencies; allowing us to 
    # query for dependencies once and reusing them twice.

    # Detach both, since derive only create shallow copies.
    ctx.load("clang_compilation_database")
    ctx.env.append_unique("CFLAGS", "-g")

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
        "platform/c/private/log.c",
        "platform/c/private/window.c",
        "platform/c/private/window_common.c",
        "assets/c/private/mesh.c",
        "assets/c/private/image.c"
    ]

    linux_sources = [
        "platform/c/private/linux/input_key_map_xcb.c",
        "platform/c/private/linux/window_xcb.c"
    ]

    macOS_sources = [
        "platform/c/private/macOS/input_key_map_appkit.c",
        "platform/c/private/macOS/window_appkit.m",
    ]

    win32_sources = [
        "platform/c/private/win32/input_key_map_win32.c",
        "platform/c/private/win32/window_win32.c"
    ]

    includes = ["platform/c/public", "assets/c/public"]

    if "ogl" not in ctx.env.EXCLUDED_FEATURES and not ctx.target_is_macOS():
        includes.append("opengl/c/public")

        sources += [
            "opengl/c/private/gl_context.c"
        ]

        win32_sources += [
            "opengl/c/private/win32/gl_context_win32.c"
        ]

        linux_sources += [
            "opengl/c/private/linux/gl_context_glx.c"
        ]

    if "gfx" not in ctx.env.EXCLUDED_FEATURES: 
        includes.append("graphics/c/public")
        
        sources += [
            "graphics/c/private/graphics.c",
            "graphics/c/private/graphics_common.c"
        ]

        macOS_sources += [
            "graphics/c/private/metal/graphics_metal.m"
        ]

    if ctx.target_is_linux():
        sources += linux_sources
    elif ctx.target_is_macOS():
        sources += macOS_sources
    elif ctx.target_is_win32():
        sources += win32_sources
    else:
        ctx.fatal("Trying to build for unknown target OS.")

    cflags = []
    if ctx.cc_is_gcc_kind():
        cflags.append("-std=c11")

    ctx.shlib(
        target="fundament",
        source=sources,
        includes=includes,
        export_includes=includes,
        defines="FUNDAMENT_EXPORTS",
        use="fundament_deps",
        cflags=cflags
    )
    
    ctx.recurse("playground")
    
