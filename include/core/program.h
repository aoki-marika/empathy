#pragma once

#include "shader.h"
#include "texture.h"
#include "matrix.h"

///
/// Programs are a set of graphical shaders which have been linked together to form a singular pipeline.
///
/// To draw with a program it must be used, which indicates to all the succeeding draw calls to draw with it.
///

// MARK: - Macros

/// The maximum number of uniform locations that a single program can cache concurrently.
#define PROGRAM_MAX_CACHED_UNIFORMS (6)

// MARK: - Data Structures

/// A graphical shader program.
struct program_t
{
    /// The unique OpenGL identifier of this program's backing.
    GLuint id;

    /// The total number of the currently cached uniform locations within this program.
    unsigned int num_cached_uniforms;

    /// All the currently cached uniform locations within this program.
    struct program_uniform_t
    {
        /// The name of this uniform within the containing program.
        ///
        /// Allocated.
        char *name;

        /// The unique OpenGL identifier of this uniform's location within the containing program.
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

/// Deinitialize the given program, releasing all of its allocated resources.
/// @param program The program to deinitialize.
void program_deinit(struct program_t *program);

/// Set the given program to be used for all succeeding draw calls.
///
/// Only one program at a time can be set to be used.
/// @param program The program to use.
void program_use(struct program_t *program);

///
/// All uniform setter functions follow the same rules and conventions,
/// so they are summarised once here and not individually documented.
///
/// `program_set_[type](struct program_t, const char *, [value type] value)`:
/// Set the given named [type] uniform of the given program to the given [value type].
///
/// If the given uniform name cannot be located within the given program then the program terminates.
/// The given program is set to be used during this function.
/// @param program The program to set the uniform of.
/// @param name The name of the [type] uniform to set.
/// @param value The [value type] to set the given [type] uniform to.
///
/// Note that [type] corresponds to the name of the GLSL type that the function sets.
///

void program_set_sampler2D(struct program_t *program,
                           const char *name,
                           unsigned int unit);

void program_set_sampler2DArray(struct program_t *program,
                                const char *name,
                                unsigned int unit);

void program_set_mat4(struct program_t *program,
                      const char *name,
                      const struct matrix4_t *value);
