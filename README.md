# fundament
*Fundament* is a foundation for game developer's to develop their games on.

## What is fundament?
It is a library, providing low-level utilities for developers to develop their
ideas with.
It is *NOT* a game engine rather than a foundation to build an engine upon.

## What does it provide?
Currently it provides a thin abstraction layer, allowing users to conveniently
create windows and to receive input events.

## What is planned for the future?
The next step is to provide further utilities for loading files formats,
including:
- Geometry: .obj, .ply
- Images: .bmp, .tga

After this has been achieved, the next step is to provide a thin layer above 
graphics APIs such as DirectX 12, Metal 2/3 and Vulkan. The goal of this layer
is provide user's with one API, without having them to worry about the
underlying API, **WHILE** providing full access to the underlying API, if 
required.

This allows users to create demanding graphics applications for multiple 
platforms, without dealing with platform specific details. But this also allows
for users to leverage features of the platform.

For instance, geometry shader's will not be supported, since Metal doesn't have
them. A user may choose to stick to compute shaders, or use geometry shaders 
exclusively on windows, if it provides an advantage in terms of performance.

## How do I build the library?
The build system used is [waf](https://waf.io), which requires python 3.

```
> python3 waf configure
> python3 waf 
```

The first command instructs waf to configure the build, checking if all dependencies
are present or not.

Generally, the default toolchain (i.e. XCode on macOS and MSVC on Windows) are 
sufficient. For linux few libraries are required.

### Dependencies in linux
The following libraries are required:
- xlib-xcb
- xcb
- xcb-xinput

## Where can I get prebuilt binaries?
Currently no prebuilt binaries are provided yet. It is planned to provide
NuGet packages for Visual Studio users, maven packages for gradle users
as well as packages in the [releases](https://github.com/HiImJulien/fundament/releases)
with 'CMakeConfig' files, allowing users to consume the library from within CMake.

Additionally, although, not set in stone yet, packages for Conan and Meson may
be provided.

## How do I use this library?
Sorry, I didn't have time to write either good samples or a good tutorial. It
is planned to provide tutorials in the repositories [wiki](https://github.com/HiImJulien/fundament/wiki).


