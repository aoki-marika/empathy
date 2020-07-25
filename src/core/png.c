#include "png.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <libpng/png.h>

// MARK: - Functions

void png_init_file(struct png_t *png, FILE *file)
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

void png_deinit(struct png_t *png)
{
    free(png->data);
}
