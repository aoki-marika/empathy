#pragma once

#include "gl.h"
#include "png.h"

///
/// Textures manage every aspect of texture images; reading, decoding, and uploading/binding to graphics contexts.
///
/// When binding a texture a texture unit is specified.
/// Using different units allows multiple textures to be used simultaneously in a single draw call or across multiple without re-binding.
/// It is generally recommended to reserve a number of texture units to be "dynamic units" that will be re-bound many times.
/// The remaining texture units can then be dedicated to large multiple-use textures like font atlases.
///

// MARK: - Macros

/// The maximum number of texture units that textures can be uploaded to.
#define TEXTURE_MAX_UNITS 16

/// The special texture unit used to initialize textures within.
///
/// OpenGL requires a unit to create a texture image, so this one is dedicated to that task.
/// This unit can still be bound to by textures, though it is rarely recommended.
#define TEXTURE_INIT_UNIT 0

// MARK: - Data Structures

/// A single texture image.
struct texture_t
{
    /// The width of this texture, in pixels.
    unsigned int width;

    /// The height of this texture, in pixels.
    unsigned int height;

    /// The filter used to scale this texture up and down when drawing.
    enum texture_scaling_t
    {
        /// Nearest neighbour.
        TEXTURE_NEAREST = 0x0,

        /// Linear interpolation.
        TEXTURE_LINEAR = 0x1,
    } scaling;

    /// The format of this texture's data.
    enum texture_format_t
    {
        /// 8-bit unsigned red, green, and blue channels.
        TEXTURE_RGBU8,

        /// 8-bit unsigned red, green, blue, and alpha channels.
        TEXTURE_RGBAU8,
    } format;

    /// The unique OpenGL identifier of this texture.
    GLuint id;
};

// MARK: - Functions

/// Initialize the given texture with the contents of the given PNG, and the given parameters.
///
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param texture The texture to initialize.
/// @param png The PNG containing the contents of the new texture.
/// @param scaling The scaling filter for the new texture to use.
void texture_init_png(struct texture_t *texture,
                      const struct png_t *png,
                      enum texture_scaling_t scaling);

/// Initialize the given texture with an empty texture image from the given parameters.
///
/// The new texture image is filled with zeroes, so depending on the format the outcome will be different:
///  - `TEXTURE_RGBU8`: Solid black.
///  - `TEXTURE_RGBAU8`: Transparent.
///
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param texture The texture to initialize.
/// @param width The width of the new texture.
/// @param height The height of the new texture.
/// @param scaling The scaling filter for the new texture to use.
/// @param format The format of the new texture's data.
void texture_init_empty(struct texture_t *texture,
                        unsigned int width,
                        unsigned int height,
                        enum texture_scaling_t scaling,
                        enum texture_format_t format);

/// Deinitialize the given texture, releasing all of its allocated resources.
///
/// It is assumed that the graphics context which the given texture was created within is current during this function.
/// @param texture The texture to deinitialize.
void texture_deinit(struct texture_t *texture);

/// Bind the given texture to the given texture within the current graphics context.
///
/// This function must be called at least once before the given texture can be used for drawing.
/// This overwrites any previously bound texture within the given unit.
/// If the given unit is greater than or equal to `TEXTURE_MAX_UNITS` then an assertion fails.
/// During this function the given unit is activated and bound to.
/// @param texture The texture to bind.
/// @param unit The unit to bind the given texture to.
void texture_bind(struct texture_t *texture, unsigned int unit);
