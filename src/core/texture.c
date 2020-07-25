#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <libpng/png.h>

// MARK: - Data Structures

/// A read PNG image.
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

    /// The decoded data of this PNG in it's format.
    ///
    /// Rows are ordered bottom-to-top.
    /// Allocated.
    void *data;
};

// MARK: - Functions

/// Read the PNG file at the current cursor of the given file handle into the given PNG.
///
/// If the PNG's colour type cannot be converted to a known format then the program terminates.
/// If there is no valid PNG file at the current cursor of the given file handle then the program terminates.
/// @param png The PNG to read the PNG file into.
/// @param file The file handle to read the PNG file from.
void png_init(struct png_t *png, FILE *file)
{
    // check the signature
    char signature[8];
    fread(signature, sizeof(signature), 1, file);
    if (png_sig_cmp(signature, 0, 8))
    {
        // the signature check failed, print the details and terminate
        fprintf(stderr, "PNG ERROR: invalid signature\n");
        exit(EXIT_FAILURE);
    }

    // open the png file for reading
    png_structp reader = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(reader);
    setjmp(png_jmpbuf(reader));
    png_init_io(reader, file);
    png_set_sig_bytes(reader, sizeof(signature));

    // read the png file
    // setup transforms to force conversion to 8-bit rgb(a) when reading
    //  - PNG_TRANSFORM_STRIP_16: strip the second byte from 16-bit channels
    //  - PNG_TRANSFORM_PACKING: use 1-byte channels for 1, 2, or 4-bit channels
    //  - PNG_TRANSFORM_EXPAND: expand data to 24-bit rgb/32-bit rgba/8-bit greyscale/16-bit greyscale with alpha
    png_read_png(reader, info, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

    // get the needed png file info
    png_uint_32 width, height;
    int bit_depth, colour_type;
    png_get_IHDR(reader, info, &width, &height, &bit_depth, &colour_type, NULL, NULL, NULL);

    // only 8-bit depth should be possible
    assert(bit_depth == 8);

    // convert the png files colour type to a known format
    enum png_format_t format;
    switch (colour_type)
    {
        case PNG_COLOR_TYPE_RGB:  format = PNG_RGBU8; break;
        case PNG_COLOR_TYPE_RGBA: format = PNG_RGBAU8; break;
        default:
            // the colour type could not be converted to a texture format, print the details and terminate
            fprintf(stderr, "TEXTURE ERROR: could not convert png colour type %i to texture format\n", colour_type);
            exit(EXIT_FAILURE);
            break;
    }

    // get the png files data
    // flip the rows from top-to-bottom to bottom-to-top for easier usage in opengl
    size_t row_size = png_get_rowbytes(reader, info);
    void *data = malloc(height * row_size);
    png_bytepp rows = png_get_rows(reader, info);
    for (int row = 0; row < height; row++)
        memcpy(data + (row * row_size), rows[(height - 1) - row], row_size);

    // initialize the given png
    png->width = width;
    png->height = height;
    png->format = format;
    png->data = data;

    // close the png file
    png_destroy_read_struct(&reader, &info, NULL);
}

/// Deinitialize the given PNG, releasing all of it's allocated resources.
/// @param png The PNG to deinitialize.
void png_deinit(struct png_t *png)
{
    free(png->data);
}

/// Create the graphics representation of the given PNG and load it into the given texture.
/// @param texture The texture to load the PNG's graphcis representation into.
/// @param png The PNG to create the graphics representation of.
void texture_create(struct texture_t *texture, struct png_t *png)
{
    // activate the init unit to bind the new texture to when creating it
    glActiveTexture(GL_TEXTURE0 + TEXTURE_INIT_UNIT);

    // get the opengl representations of the various properties
    GLenum filter;
    switch (texture->scaling)
    {
        case TEXTURE_NEAREST:
            filter = GL_NEAREST;
            break;
        case TEXTURE_LINEAR:
            filter = GL_LINEAR;
            break;
    }

    GLenum internal_format, format, type;
    switch (png->format)
    {
        case PNG_RGBU8:
            internal_format = GL_RGB;
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            break;
        case PNG_RGBAU8:
            internal_format = GL_RGBA;
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
    }

    // generate the texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, png->width, png->height, 0, format, type, png->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glGenerateMipmap(GL_TEXTURE_2D);

    // initialize the given texture
    texture->width = png->width;
    texture->height = png->height;
    texture->id = id;
}

void texture_init(struct texture_t *texture,
                  const char *path,
                  long offset,
                  enum texture_scaling_t scaling)
{
    // open the given file
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        // the given path could not be opened, print the details and terminate
        fprintf(stderr, "TEXTURE ERROR: could not open file \"%s\" (%s)\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // initialize the given texture
    texture->scaling = scaling;

    // read the png and create the graphics representation within the given texture
    struct png_t png;
    fseek(file, offset, SEEK_SET);
    png_init(&png, file);
    texture_create(texture, &png);

    // deinit everything only used for loading
    png_deinit(&png);
    fclose(file);
}

void texture_deinit(struct texture_t *texture)
{
    glDeleteTextures(1, &texture->id);
}

void texture_bind(struct texture_t *texture, unsigned int unit)
{
    // ensure the given unit it valid
    assert(unit < TEXTURE_MAX_UNITS);

    // activate and bind to the given unit
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}
