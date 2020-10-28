# (Emp)athy

A project with abstract goals of communicating and exploring emotion.

## Development

### Structure

This project is split into several smaller components:

 - `core`: The internal engine library.
 - `sys2d`: The 2D subsystem.
 - `game`: The end user game.

### Requirements

Toolchain:
 - GCC and G++
    - Debian: `build-essential`
    - MSYS2: `mingw-w64-x86_64-gcc`
 - GNU Make
     - Debian: `build-essential`
     - MSYS2: `mingw-w64-x86_64-make`
 - CMake
     - Debian: `cmake`
     - MSYS2: `mingw-w64-x86_64-cmake`

Dependencies:
 - `core`:
    - `cimgui` (included submodule)
    - GLEW (`2.0`)
       - Debian: `libglew-dev`
       - MSYS2: `mingw-w64-x86_64-glew`
    - GLFW (`3.3`)
       - Debian: `libglfw3-dev`
       - MSYS2: `mingw-w64-x86_64-glfw`
    - libpng (`1.6`)
       - Debian: `libpng-dev`
       - MSYS2: `mingw-w64-x86_64-libpng`
 - `sys2d`:
    - `core` (included)
 - `game`:
    - `core` (included)
    - `sys2d` (included)

### Building

#### Windows

 - Install [MSYS2](https://www.msys2.org/).
 - Install the required toolchain and dependencies with `pacman`.
 - **Command line**:
    - Run `mingw32-make.exe [component] DEBUG=[0/1]` from the root of the repository within an MSYS2 MinGW 64-bit shell.
    - The build output is within `bin/`.
 - **Visual Studio Code**:
    - A VS Code workspace is provided for a more integrated development experience on Windows.
    - Add the MinGW 64-bit bin (`C:\msys64\mingw64\bin` with the default MSYS2 install path) to the Windows `PATH` environment variable.
       - See [this document](https://code.visualstudio.com/docs/cpp/config-mingw#_prerequisites) if you're unsure how.
    - Run the `Game (Debug)` or `Game (Release)` targets to compile and execute the `game` target in the respective configuration.
       - The `Build Game (Debug)` and `Build Game (Release)` tasks can be used directly to build only.

#### Linux

 - Install the required toolchain and dependencies.
 - Run `make [component] DEBUG=[0/1]` from the root of the repository.
 - The build output is within `bin/`.

 ---

 Note that the `clean` target, or the `Clean` task in VS Code, must be performed when switching between debug and release.
 This ensures all sources are recompiled for the respective target.