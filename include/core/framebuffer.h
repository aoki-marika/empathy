#pragma once

#include <stdbool.h>

#include "texture.h"
#include "colour.h"

///
/// Framebuffers are wrappers around rendering to textures, used to apply things like post-processing effects.
///

// MARK: - Data Structures

/// A framebuffer which can be rendered to.
struct framebuffer_t
{
    /// The unique OpenGL identifier of this framebuffer.
    GLuint id;

    /// The texture that this framebuffer renders to.
    struct texture_t texture;

    /// Whether or not this framebuffer has a set background.
    bool has_background;

    /// The set background colour of this framebuffer, if any.
    struct colour4_t background_colour;
};

// MARK: - Functions

/// Initialize the given framebuffer of the given size.
///
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// During this function the current render target is reset.
/// @param framebuffer The framebuffer to initialize.
/// @param width The width, in pixels, of the new framebuffer's render texture.
/// @param height The height, in pixels, of the new framebuffer's render texture.
void framebuffer_init(struct framebuffer_t *framebuffer,
                      unsigned int width,
                      unsigned int height);

/// Deinitialize the given framebuffer, releasing all of it's allocated resources.
/// @param framebuffer The framebuffer to deinitialize.
void framebuffer_deinit(struct framebuffer_t *framebuffer);

/// Set the background of the given framebuffer to the given colour.
///
/// This colour is what fills the given framebuffer when it is used.
/// If this is not called on the given framebuffer then the window background is used instead.
/// @param framebuffer The framebuffer to set the background of.
/// @param colour The colour to set the background to.
void framebuffer_set_background(struct framebuffer_t *framebuffer, struct colour4_t colour);

/// Set the given framebuffer as the render target within the current graphics context, and clear it.
///
/// During this function the given framebuffer's viewport is applied to the current graphics context.
/// @param framebuffer The framebuffer to set as the render target.
void framebuffer_use(struct framebuffer_t *framebuffer);
