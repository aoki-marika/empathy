# (Emp)athy

An experimental music game based on the concept of communicating music through gameplay, aesthetically inspired by the underground electronic music scene.

## Development

### Structure

This project is split into several smaller components:

 - `core`: The internal engine library.
 - `game`: The end user game.
 - `design`: The developer program for creating interfaces.

### Building

Essentials:
 - `gcc`, `make`, and the C standard library are required for all targets.
 - `g++`, `cmake`, and the C++ standard library are required for `design` to build `cimgui`.

Dependencies:
 - `core`:
    - `libglfw3-dev`
 - `game`:
    - `core` (included)
 - `design`:
    - `core` (included)
    - `cimgui` (included submodule)
