#pragma once

#include <stdio.h>

///
/// Utility wrapper around working with PNG images.
///

// MARK: - Forward Declarations

struct texture_t;

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

    /// The data of this PNG, in its format.
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

/// Initialize the given PNG with the contents of the given 2D texture.
///
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// If the given texture is not a 2D texture then an assertion fails.
/// @param png The PNG to initialize.
/// @param texture The texture to use the contents of for the new PNG.
void png_init_texture(struct png_t *png, const struct texture_t *texture);

/// Deinitialize the given PNG, releasing all of its allocated resources.
/// @param png The PNG to deinitialize.
void png_deinit(struct png_t *png);

/// Write the given PNG to the current cursor of the given file handle.
/// @param png The PNG to write.
/// @param file The file handle to write the PNG file to.
void png_write(struct png_t *png, FILE *file);
