#pragma once

#include "texture.h"

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
};

// MARK: - Functions

/// Initialize the given framebuffer of the given size.
///
/// During this function `TEXTURE_INIT_UNIT` as activated and bound to.
/// During this function the new framebuffer is set as the render target.
/// @param framebuffer The framebuffer to initialize.
/// @param width The width, in pixels, of the new framebuffer's render texture.
/// @param height The height, in pixels, of the new framebuffer's render texture.
void framebuffer_init(struct framebuffer_t *framebuffer,
                      unsigned int width,
                      unsigned int height);

/// Deinitialize the given framebuffer, releasing all of it's allocated resources.
/// @param framebuffer The framebuffer to deinitialize.
void framebuffer_deinit(struct framebuffer_t *framebuffer);

/// Set the given framebuffer as the render target within the current graphics context, and clear it.
/// @param framebuffer The framebuffer to set as the render target.
void framebuffer_use(struct framebuffer_t *framebuffer);
