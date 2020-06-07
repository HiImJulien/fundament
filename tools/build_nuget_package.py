"""
    This script creates a NuGet package for fundament library.
"""

import subprocess

# The install dir for the artifacts
RELEASE_OUTPUT_DIR = "./build/release"
DEBUG_OUTPUT_DIR = "./build/debug"

# The build dir
RELEASE_BUILD_DIR = "./build/bin/release"
DEBUG_BUILD_DIR = "./build/bin/debug"

def build_debug():
    subprocess.run(["qbs", "install", "-d", DEBUG_BUILD_DIR,"--install-root", DEBUG_OUTPUT_DIR, "config:debug"])

def build_release():
    subprocess.run(["qbs", "install", "-d", RELEASE_BUILD_DIR, "--install-root", RELEASE_OUTPUT_DIR, "config:release"])

def create_package():
    subprocess.run(["nuget", "pack", "./tools/fundament.nuspec"])

def main():
    build_debug()
    build_release()
    create_package()

    pass

if __name__ == '__main__':
    main()
