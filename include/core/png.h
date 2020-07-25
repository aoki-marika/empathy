#pragma once

#include <stdio.h>

#include "texture.h"

///
/// Utility wrapper around working with PNG images.
///

// MARK: - Data Structures

/// A PNG image.
struct png_t
{
    /// The width of this PNG, in pixels.
    unsigned int width;

    /// The height of this PNG, in pixels.
    unsigned int height;

    /// The format of this PNG's data.
    enum png_format_t
    {
        /// 8-bit unsigned red, green, and blue channels.
        PNG_RGBU8,

        /// 8-bit unsigned red, green, blue, and alpha channels.
        PNG_RGBAU8,
    } format;

    /// The data of this PNG, in it's format.
    ///
    /// Rows are ordered bottom-to-top.
    /// Allocated.
    void *data;

};

// MARK: - Functions

/// Initialize the given PNG from the PNG file at the current cursor of the given file handle.
///
/// If there is no valid PNG file at the current cursor of the given file handle then the program terminates.
/// If the PNG file's colour type cannot be converted to a known format then the program terminates.
/// @param png The PNG to initialize.
/// @param file The file handle to read the PNG file from.
void png_init_file(struct png_t *png, FILE *file);

/// Initialize the given PNG from the contents of the given texture, in the given format.
///
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param png The PNG to initialize.
/// @param texture The texture to use the contents of for the new PNG.
/// @param format The format to use for the new PNG.
void png_init_texture(struct png_t *png,
                      struct texture_t *texture,
                      enum png_format_t format);

/// Deinitialize the given PNG, releasing all of it's allocated resources.
/// @param png The PNG to deinitialize.
void png_deinit(struct png_t *png);
