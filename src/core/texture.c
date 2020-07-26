#include "texture.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "png.h"

// MARK: - Functions

/// Create and return a new OpenGL texture from the given parameters.
///
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param width The width of the new texture, in pixels.
/// @param height The height of the new texture, in pixels.
/// @param scaling The scaling filter for the new texture to use.
/// @param format The format of the new texture's data.
/// @param data The data to create the new texture with.
/// It is assumed that this data is allocated and set properly, no checks are performed.
/// @param The unique OpenGL identifier of the new texture.
GLuint texture_create(unsigned int width,
                      unsigned int height,
                      enum texture_scaling_t scaling,
                      enum texture_format_t format,
                      void *data)
{
    // activate the init unit to bind the new texture to when creating it
    glActiveTexture(GL_TEXTURE0 + TEXTURE_INIT_UNIT);

    // get the opengl representations of the various properties
    GLenum gl_filter;
    switch (scaling)
    {
        case TEXTURE_NEAREST:
            gl_filter = GL_NEAREST;
            break;
        case TEXTURE_LINEAR:
            gl_filter = GL_LINEAR;
            break;
    }

    GLenum gl_internal_format, gl_format, gl_type;
    switch (format)
    {
        case TEXTURE_RGBU8:
            gl_internal_format = GL_RGB;
            gl_format = GL_RGB;
            gl_type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_RGBAU8:
            gl_internal_format = GL_RGBA;
            gl_format = GL_RGBA;
            gl_type = GL_UNSIGNED_BYTE;
            break;
    }

    // generate and configure the new texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, gl_format, gl_type, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    glGenerateMipmap(GL_TEXTURE_2D);

    // return the new texture
    return id;
}

void texture_init_png(struct texture_t *texture,
                      const struct png_t *png,
                      enum texture_scaling_t scaling)
{
    // map the given pngs format to its respective texture format
    enum texture_format_t texture_format;
    switch (png->format)
    {
        case PNG_RGBU8:  texture_format = TEXTURE_RGBU8; break;
        case PNG_RGBAU8: texture_format = TEXTURE_RGBAU8; break;
    }

    // create the new texture and initialize the given texture
    GLuint id = texture_create(png->width, png->height, scaling, texture_format, png->data);
    texture->width = png->width;
    texture->height = png->height;
    texture->scaling = scaling;
    texture->format = texture_format;
    texture->id = id;
}

void texture_init_empty(struct texture_t *texture,
                        unsigned int width,
                        unsigned int height,
                        enum texture_scaling_t scaling,
                        enum texture_format_t format)
{
    // get the size of each pixel
    size_t pixel_size;
    switch (format)
    {
        case TEXTURE_RGBU8:  pixel_size = 3; break;
        case TEXTURE_RGBAU8: pixel_size = 4; break;
    }

    // create the empty data
    size_t data_size = width * height * pixel_size;
    void *data = malloc(data_size);
    memset(data, 0x0, data_size);

    // create the new texture and initialize the given texture
    GLuint id = texture_create(width, height, scaling, format, data);
    texture->width = width;
    texture->height = height;
    texture->scaling = scaling;
    texture->format = format;
    texture->id = id;

    // free the empty data as its now uploaded
    free(data);
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
