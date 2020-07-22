#pragma once

#include "shader.h"

///
/// Programs are used to manage OpenGL shader programs.
///

// MARK: - Data Structures

/// An OpenGL shader program.
struct program_t
{
    /// Unique OpenGL identifier of this program's backing.
    GLuint id;
};

// MARK: - Functions

/// Initialize the given program, attaching the given shaders to it.
///
/// If there are any linker errors then the program terminates.
/// It is expected that the given shaders remain available for the entire lifetime of the given program.
/// @param program The program to initialize.
/// @param num_shaders The total number of shaders to attach to the new programn.
/// @param shaders All the shaders to attach to the new program.
void program_init(struct program_t *program,
                  unsigned int num_shaders,
                  const struct shader_t *shaders);

/// Deinitialize the given program, releasing all of it's allocated resources.
/// @param program The program to deinitialize.
void program_deinit(struct program_t *program);
