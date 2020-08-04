# (Emp)athy

An experimental music game based on the concept of communicating music through gameplay, aesthetically inspired by the underground electronic music scene.

## Development

### Structure

This project is split into several smaller components:

 - `core`: The internal engine library.
 - `sys2d`: The 2D subsystem.
 - `game`: The end user game.

### Building

Essentials:
 - `gcc`, `make`, and the C standard library are required for all targets.
 - `g++`, `cmake`, and the C++ standard library are required for all targets to build and link `cimgui`.

Dependencies:
 - `core`:
    - `cimgui` (included submodule)
    - `libglfw3-dev` (`3.3.2-1`)
    - `libpng-dev` (`1.6.36-6`)
 - `sys2d`:
    - `core` (included)
 - `game`:
    - `core` (included)
    - `sys2d` (included)

Each component can be built with `make [component] DEBUG=[0/1]` from the root of the project.
If no `DEBUG` option is supplied then a release build is compiled.

Build output can be deleted with `make clean`.

**Note**: When switching between different `DEBUG` options `make clean` must be run to ensure that everything is rebuilt with the new flags.
