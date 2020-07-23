#pragma once

#include "shader.h"
#include "texture.h"

///
/// Programs are used to manage OpenGL shader programs.
///

// MARK: - Macros

/// The maximum number of uniform locations that a single program can cache concurrently.
#define PROGRAM_MAX_CACHED_UNIFORMS 6

// MARK: - Data Structures

/// An OpenGL shader program.
struct program_t
{
    /// Unique OpenGL identifier of this program's backing.
    GLuint id;

    /// The total number of the currently cached uniform locations within this program.
    unsigned int num_cached_uniforms;

    /// All the currently cached uniform locations within this program.
    struct program_uniform_t
    {
        /// The name of this uniform within the program.
        ///
        /// Allocated.
        char *name;

        /// The unique OpenGL identifier of this uniform's location within it's program.
        GLuint location;
    } cached_uniforms[PROGRAM_MAX_CACHED_UNIFORMS];
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

/// Set the given program to be used for drawing.
///
/// This must be called again if another program is set to be used.
/// @param program The program to use.
void program_use(struct program_t *program);

/// Set the given named sampler2D uniform of the given program to the given texture.
///
/// The given program is set to be used during this function.
/// @param program The program to set the uniform of.
/// @param name The name of the sampler2D uniform to set.
/// @param texture The texture to set the given sampler2D uniform to.
void program_set_sampler2d(struct program_t *program,
                           const char *name,
                           const struct texture_t *texture);
