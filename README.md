# (Emp)athy

An experimental music game based on the concept of communicating music through gameplay, aesthetically inspired by the underground electronic music scene.

## Development

### Structure

This project is split into several smaller components:

 - `core`: The internal engine library.
 - `game`: The end user game.
 - `design`: The developer program for creating assets.

### Building

Essentials:
 - `gcc`, `make`, and the C standard library are required for all targets.
 - `g++`, `cmake`, and the C++ standard library are required for `design` to build `cimgui`.

Dependencies:
 - `core`:
    - `libglfw3-dev`
    - `libpng-dev`
 - `game`:
    - `core` (included)
 - `design`:
    - `core` (included)
    - `cimgui` (included submodule)

Each component can be built with `make [component] DEBUG=[0/1]` from the root of the project.
If no `DEBUG` option is supplied then a release build is compiled.

Build output can be deleted with `make clean`.

**Note**: When switching between different `DEBUG` options `make clean` must be run to ensure that everything is rebuilt with the new flags.
