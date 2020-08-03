#pragma once

#include "program.h"
#include "mesh.h"
#include "instance.h"

///
/// A fullscreen instance output displays the final framebuffer of the instance's program within the entire bounds of the window.
///
/// Generally when using a fullscreen instace output the window size should be the same as the instance's render size and non-resizable,
/// to avoid any stretching or artifacts.
///

// MARK: - Macros

/// The texture unit that a fullscreen instance output binds the instance's program's final framebuffer texture to.
///
/// `TEXTURE_INIT_UNIT` is used as to not take any available texture units from the instance's program.
#define INSTANCE_OUTPUT_FULLSCREEN_FRAMEBUFFER_UNIT TEXTURE_INIT_UNIT

// MARK: - Data Structures

/// A fullscreen instance output.
struct instance_output_fullscreen_t
{
    /// The backing output of this output.
    ///
    /// This is the actual output which is passed to an instance.
    struct instance_output_t backing;

    /// The shaders for this output's program.
    ///
    /// Managed by the backing output's functions.
    struct shader_t shaders[2];

    /// The program that this output uses to render its final framebuffer mesh.
    ///
    /// Managed by the backing output's functions.
    struct program_t program;

    /// The mesh that this output uses to render an instance's final framebuffer.
    ///
    /// Managed by the backing output's functions.
    struct mesh_t mesh;
};

// MARK: - Functions

/// Initialize the given fullscreen instance output.
/// @param output The output to initialize.
void instance_output_fullscreen_init(struct instance_output_fullscreen_t *output);

/// Deinitialize the given fullscreen instance output, releasing all of its allocated resources.
/// @param output The output to deinitialize.
void instance_output_fullscreen_deinit(struct instance_output_fullscreen_t *output);
