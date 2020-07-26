#include "texture.h"

#include <assert.h>

#include "png.h"

// MARK: - Functions

void texture_init_png(struct texture_t *texture,
                      const struct png_t *png,
                      enum texture_scaling_t scaling)
{
    // activate the init unit to bind the new texture to when creating it
    glActiveTexture(GL_TEXTURE0 + TEXTURE_INIT_UNIT);

    // get the opengl representations of the various properties
    GLenum gl_filter;
    switch (texture->scaling)
    {
        case TEXTURE_NEAREST:
            gl_filter = GL_NEAREST;
            break;
        case TEXTURE_LINEAR:
            gl_filter = GL_LINEAR;
            break;
    }

    // also map the png format to its respective texture format
    GLenum gl_internal_format, gl_format, gl_type;
    enum texture_format_t format;
    switch (png->format)
    {
        case PNG_RGBU8:
            gl_internal_format = GL_RGB;
            gl_format = GL_RGB;
            gl_type = GL_UNSIGNED_BYTE;
            format = TEXTURE_RGBU8;
            break;
        case PNG_RGBAU8:
            gl_internal_format = GL_RGBA;
            gl_format = GL_RGBA;
            gl_type = GL_UNSIGNED_BYTE;
            format = TEXTURE_RGBAU8;
            break;
    }

    // generate the texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_format, png->width, png->height, 0, gl_format, gl_type, png->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    glGenerateMipmap(GL_TEXTURE_2D);

    // initialize the given texture
    texture->width = png->width;
    texture->height = png->height;
    texture->scaling = scaling;
    texture->format = format;
    texture->id = id;
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
