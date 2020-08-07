#pragma once

#include "gl.h"
#include "png.h"

///
/// Textures manage uploading texture images to graphics contexts and allowing usage of them.
///
/// When creating a texture it can be one of several types:
///  - 2D: A grid of pixels on a two-dimensional plane.
///        These are the default for most textures, and should generally be used unless an array is needed for optimizations.
///  - 2D Array: An array of 2D textures, indexed by a third axis on the plane.
///              These have many optimization usages such as accessing multiple textures within a single draw call or leaving more texture units available.
///              However texture arrays come with the limitation that all textures are within the width and height of the array.
///              UV coordinates are normalized to the array texture's size, and textures are placed within this size from UV 0,0.
///              For this reason it is generally recommended, though not required, to have all textures within a texture array be the same size.
///
/// When binding a texture a texture unit is specified.
/// Using different units allows multiple textures to be used simultaneously in a single draw call or across multiple without re-binding.
/// It is generally recommended to reserve a number of texture units to be "dynamic units" that will be re-bound many times.
/// The remaining texture units can then be dedicated to large multiple-use textures like font atlases.
///

// MARK: - Macros

/// The maximum number of texture units that textures can be uploaded to.
#define TEXTURE_MAX_UNITS (16)

/// The special texture unit used to initialize textures within.
///
/// OpenGL requires a unit to create a texture image, so this one is dedicated to that task.
/// This unit can still be bound to by textures, though it is rarely recommended.
#define TEXTURE_INIT_UNIT (0)

// MARK: - Data Structures

/// A single texture image.
struct texture_t
{
    /// The width of this texture, in pixels.
    unsigned int width;

    /// The height of this texture, in pixels.
    unsigned int height;

    /// The type of this texture.
    enum texture_type_t
    {
        /// A two-dimensional texture.
        TEXTURE_2D,

        /// An array of two-dimensional textures indexed by a third axis on the plane.
        TEXTURE_2D_ARRAY,
    } type;

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

/// Initialize the given texture with a 2D texture from the given PNG and parameters.
///
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param texture The texture to initialize.
/// @param scaling The scaling filter for the new texture to use.
/// @param png The PNG to create the new texture from.
void texture_init_png(struct texture_t *texture,
                      enum texture_scaling_t scaling,
                      const struct png_t *png);

/// Initialize the given texture with an array texture from the given parameters, populated with 2D textures from the given PNGs.
///
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param texture The texture to initialize.
/// @param width The width of the new array texture, in pixels.
/// @param height The height of the new array texture, in pixels.
/// @param scaling The scaling filter for the new array texture to use.
/// This scaling filter is used for the entire array texture, individual elements cannot have their own scaling filters.
/// @param num_pngs The total number of given PNGs.
/// @param pngs All the PNGs to create the new textures within the new array texture from.
void texture_init_png_array(struct texture_t *texture,
                            unsigned int width,
                            unsigned int height,
                            enum texture_scaling_t scaling,
                            unsigned int num_pngs,
                            const struct png_t *pngs);

/// Initialize the given texture with an empty 2D texture from the given parameters.
///
/// Note that the appearance of an empty texture varies depending on the format:
///  - `TEXTURE_RGBU8`: Solid black.
///  - `TEXTURE_RGBAU8`: Transparent.
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param texture The texture to initialize.
/// @param width The width of the new texture, in pixels.
/// @param height The height of the new texture, in pixels.
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
