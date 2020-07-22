#pragma once

#include "gl.h"

///
/// Textures manage texture images which are uploaded to OpenGL and used when drawing.
///
/// When creating a texture a texture unit is specified.
/// Using different units allows multiple textures to be used simultaneously in a single draw call or across multiple without re-binding.
/// It is generally recommended to reserve a number of texture units to be "dynamic units" that will be re-bound many times.
/// The remaining texture units can then be dedicated to large multiple-use textures like atlases.
///

// MARK: - Macros

/// The maximum number of units available to textures.
#define TEXTURE_MAX_UNIT 16

// MARK: - Enumerations

/// The different formats that a single texture's data can be in.
enum texture_format_t
{
    /// 8-bit unsigned red, green, and blue channels.
    TEXTURE_RGBU8 = 0,

    /// 8-bit unsigned red, green, blue, and alpha channels.
    TEXTURE_RGBAU8 = 1,
};

/// The different filters used to scale up and down a single texture.
enum texture_filter_t
{
    /// Nearest neighbour.
    TEXTURE_NEAREST = 0,

    /// Linear interpolation.
    TEXTURE_LINEAR = 1,
};

// MARK: - Data Structures

/// A texture uploaded to OpenGL.
struct texture_t
{
    /// The unique OpenGL identifier of this texture's backing.
    GLuint id;

    /// The texture unit that this texture is using.
    GLenum unit;
};

// MARK: - Functions

/// Initialize the given texture from the given parameters.
///
/// If the given texture unit was already assigned to another texture then that texture is overwritten,
/// and `texture_bind(struct texture_t *)` must be called with it to restore it.
/// The given unit is activated and the new texture is bound to it during this function.
/// @param texture The texture to initialize.
/// @param width The width, in pixels, of the new texture.
/// @param height The height, in pixels, of the new texture.
/// @param format The format of the given texture data.
/// @param filter The scaling filter for the new texture to use.
/// @param data The texture data for the new texture.
/// This data is assumed to be in left-to-right ordered rows ordered from bottom-to-top.
/// @param unit The index of the texture unit for the new texture to use.
/// This index must be less then `TEXTURE_MAX_UNIT`.
void texture_init(struct texture_t *texture,
                  unsigned int width,
                  unsigned int height,
                  enum texture_format_t format,
                  enum texture_filter_t filter,
                  unsigned int unit,
                  const void *data);

/// Deinitialize the given texture, releasing all of it's allocated resources.
/// @param texture The texture to deinitialize.
void texture_deinit(struct texture_t *texture);

/// Activate the given texture's texture unit and bind it to it.
///
/// This only needs to be called when another texture binds to the given texture's unit.
/// @param texture The texture to bind.
void texture_bind(struct texture_t *texture);
