# fundament
A slick foundation for game developers.

## About
**Fundament** provides a foundation for game developers and graphics engineers
to build their application on. The foundation consists of functions, concepts
and data structures commonly found in this type of applications.

Functionality include:
- [IN-PROGRESS]     Cross-platform window creation and management
- [TODO]            Vector math types and functions implemented ontop of SSE
- [TODO]            3D Geometry file load for the formats OBJ, PLY, glTF
- [TODO]            Lightweight/low-level abstractions for the platforms native
                    rendering API (Metal on macOS, Vulkan on linux, D3D12 on Win32)
- [TODO]            Asset manager, preventing same resources to be allocated 
                    multiple times
- [TODO]            Package API, allowing to easily access the applications
                    resources from a well-known directory.
- [TODO]            Toolkit, providing a small 'engine' for people to build
                    their prototypes on, while providing the ability to dive
                    in the platform specific details, when necessary.
- [TODO]            Sound API

Language bindings include:
- [TODO]            Python bindings
- [TODO]            Java bindings
- [TODO]            C# / .NetCore bindings
- [TODO]            Lua bindings
- [TODO]            Cxx bindings

## Tutorials
Documentation on using the library can be drawn either from the wiki
or the projects page.

## Building
### Building the base library (C)
Aside from platform specific dependencies, this projects requires a C18 
(although C11 should be sufficient) and CMake.

- On Windows, the default SDK provided with Visual Studio is enough; MinGW based
  toolchains should work fine as well, but weren't tested.
- On macOS, the default SDK provided with XCode is enough.
- On linux you need to download the 'xcb' development library.

The build is configured to provide you the following options:
- BUILD_SAMPLES (default: OFF)

These can be conveniently set in the CMake's configuration step.
As follows:
```
    cmake . -Bbuild -DBUILD_SAMPLES=ON
```

Sample command line to build the library (without samples)
```
    cmake . -Bbuild
    cmake --build build
```
