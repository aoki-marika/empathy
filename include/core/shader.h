#pragma once

#include "gl.h"

///
/// Shaders are used to manage OpenGL vertex, fragment, and geometry shaders.
///

// MARK: - Enumerations

/// The different types that a single shader can be.
enum shader_type_t
{
    /// Vertex shader.
    SHADER_VERTEX = 0,

    /// Fragment shader.
    SHADER_FRAGMENT = 1,

    /// Geometry shader.
    SHADER_GEOMETRY = 2,
};

// MARK: - Data Structures

/// An OpenGL shader.
struct shader_t
{
    /// The unique OpenGL identifier of this shader's backing.
    GLuint id;
};

// MARK: - Functions

/// Initialize the given shader by compiling the given GLSL source of the given type.
///
/// If there are any compilation errors then the program terminates.
/// @param shader The shader to initialize.
/// @param type The type of the new shader.
/// @param source The GLSL source of the given type to compile into the new shader.
void shader_init(struct shader_t *shader, enum shader_type_t type, const char *source);

/// Deinitialize the given shader, releasing all of it's allocated resources.
/// @param shader The shader to deinitialize.
void shader_deinit(struct shader_t *shader);
