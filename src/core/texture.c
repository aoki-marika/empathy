#include "texture.h"

#include <assert.h>

// MARK: - Functions

void texture_init(struct texture_t *texture,
                  unsigned int width,
                  unsigned int height,
                  enum texture_format_t format,
                  enum texture_filter_t filter,
                  unsigned int unit,
                  const void *data)
{
    // get all the opengl representations
    GLenum gl_internal_format, gl_format, gl_type;
    switch (format)
    {
        case TEXTURE_RGBU8:
            gl_internal_format = GL_RGB;
            gl_format = GL_RGB;
            gl_type = GL_UNSIGNED_BYTE;
            break;
    }

    GLenum gl_filter;
    switch (filter)
    {
        case TEXTURE_NEAREST: gl_filter = GL_NEAREST; break;
        case TEXTURE_LINEAR:  gl_filter = GL_LINEAR; break;
    }

    assert(unit < TEXTURE_MAX_UNIT);
    GLenum gl_unit = GL_TEXTURE0 + unit;

    // create the texture within the given texture unit
    GLuint id;
    glGenTextures(1, &id);
    glActiveTexture(gl_unit);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(
        GL_TEXTURE_2D, //target
        0, //level
        gl_internal_format, //internal format
        width, //width
        height, //height
        0, //border
        gl_format, //format
        gl_type, //type
        data //pixels
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    glGenerateMipmap(GL_TEXTURE_2D);

    // initialize the texture
    texture->id = id;
    texture->unit = gl_unit;
}

void texture_deinit(struct texture_t *texture)
{
    glDeleteTextures(1, &texture->id);
}

void texture_bind(struct texture_t *texture)
{
    glActiveTexture(texture->unit);
    glBindTexture(GL_TEXTURE_2D, texture->id);
}
