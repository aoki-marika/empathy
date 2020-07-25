#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include "png.h"

// MARK: - Functions

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
    png_init_file(&png, file);
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
