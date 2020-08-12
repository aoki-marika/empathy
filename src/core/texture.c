#include "texture.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "png.h"

// MARK: - Functions

/// Get the OpenGL representation of the given texture type.
/// @param type The type to get the OpenGL representation of.
/// @param gl_target The pointer to set the value of to the texture target for the given type.
void texture_type_to_gl(enum texture_type_t type,
                        GLenum *gl_target)
{
    switch (type)
    {
        case TEXTURE_2D:
            *gl_target = GL_TEXTURE_2D;
            break;
        case TEXTURE_2D_ARRAY:
            *gl_target = GL_TEXTURE_2D_ARRAY;
            break;
    }
}

/// Get the OpenGL representation of the given texture scaling.
/// @param scaling The scaling to get the OpenGL representation of.
/// @param gl_filter The pointer to set the value of to the texture filter for the given scaling.
void texture_scaling_to_gl(enum texture_scaling_t scaling,
                           GLenum *gl_filter)
{
    switch (scaling)
    {
        case TEXTURE_NEAREST:
            *gl_filter = GL_NEAREST;
            break;
        case TEXTURE_LINEAR:
            *gl_filter = GL_LINEAR;
            break;
    }
}

/// Get the OpenGL representations of the given texture format.
/// @param format The format to get the OpenGL representation of.
/// @param gl_internal_format The pointer to set the value of to the internal texture format for the given type.
/// @param gl_format The pointer to set the value of to the texture format for the given type.
/// @param gl_type The pointer to set the value of to the texture data type for the given type.
void texture_format_to_gl(enum texture_format_t format,
                          GLenum *gl_internal_format,
                          GLenum *gl_format,
                          GLenum *gl_type)
{
    switch (format)
    {
        case TEXTURE_RGBU8:
            *gl_internal_format = GL_RGB;
            *gl_format = GL_RGB;
            *gl_type = GL_UNSIGNED_BYTE;
            break;
        case TEXTURE_RGBAU8:
            *gl_internal_format = GL_RGBA;
            *gl_format = GL_RGBA;
            *gl_type = GL_UNSIGNED_BYTE;
            break;
    }
}

/// Get the texture representation of the given PNG format.
/// @param png_format The PNG format to get the texture representation of.
/// @return The texture representation of the given PNG format.
enum texture_format_t texture_format_from_png(enum png_format_t png_format)
{
    switch (png_format)
    {
        case PNG_RGBU8:
            return TEXTURE_RGBU8;
        case PNG_RGBAU8:
            return TEXTURE_RGBAU8;
    }
}

/// Activate the given indexed texture unit within the current graphics context.
/// @param index The index of the texture unit to activate.
void texture_activate_unit(unsigned int index)
{
    glActiveTexture(GL_TEXTURE0 + index);
}

/// Create a new unpopulated OpenGL texture from the given parameters.
///
/// As the new texture is unpopulated is must be populated by the caller, then optionally have its mipmap generated.
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param type The type of the new texture.
/// @param scaling The scaling filter for the new texture to use.
/// @return The unique OpenGL identifier of the new texture.
GLuint texture_create(enum texture_type_t type,
                      enum texture_scaling_t scaling)
{
    // get the opengl representations of the new textures properties
    GLenum gl_target, gl_filter;
    texture_type_to_gl(type, &gl_target);
    texture_scaling_to_gl(type, &gl_filter);

    // activate the init unit
    texture_activate_unit(TEXTURE_INIT_UNIT);

    // create and return the new texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(gl_target, id);
    glTexParameteri(gl_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(gl_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(gl_target, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(gl_target, GL_TEXTURE_MAG_FILTER, gl_filter);
    return id;
}

void texture_init_png(struct texture_t *texture,
                      enum texture_scaling_t scaling,
                      const struct png_t *png)
{
    // get the opengl representations of the new textures properties
    enum texture_type_t type = TEXTURE_2D;
    enum texture_format_t format = texture_format_from_png(png->format);
    GLenum gl_target, gl_internal_format, gl_format, gl_type;
    texture_type_to_gl(type, &gl_target);
    texture_format_to_gl(format, &gl_internal_format, &gl_format, &gl_type);

    // create and populate the new texture
    GLuint id = texture_create(type, scaling);
    glTexImage2D(gl_target,
                 0,
                 gl_internal_format,
                 png->width,
                 png->height,
                 0,
                 gl_format,
                 gl_type,
                 png->data);

    // generate the mipmap now that the texture is populated
    glGenerateMipmap(gl_target);

    // initialize the given texture
    texture->width = png->width;
    texture->height = png->height;
    texture->type = type;
    texture->scaling = scaling;
    texture->format = format;
    texture->id = id;
}

void texture_init_png_array(struct texture_t *texture,
                            unsigned int width,
                            unsigned int height,
                            enum texture_scaling_t scaling,
                            unsigned int num_pngs,
                            const struct png_t *pngs)
{
    // get whether or not any of the given pngs has an alpha channel,
    // to determine the internal format of the new array texture
    bool any_has_alpha = false;
    for (int i = 0; i < num_pngs && !any_has_alpha; i++)
    {
        const struct png_t *png = &pngs[i];
        switch (png->format)
        {
            case PNG_RGBAU8:
                any_has_alpha = true;
                break;
            default:
                break;
        }
    }

    // get the opengl representations of the new array textures properties
    enum texture_type_t type = TEXTURE_2D_ARRAY;
    enum texture_format_t array_format = (any_has_alpha) ? TEXTURE_RGBAU8 : TEXTURE_RGBU8;
    GLenum gl_target, gl_array_internal_format, gl_array_format, gl_array_type;
    texture_type_to_gl(type, &gl_target);
    texture_format_to_gl(array_format, &gl_array_internal_format, &gl_array_format, &gl_array_type);

    // create the new array texture
    GLuint id = texture_create(type, scaling);
    glTexImage3D(gl_target,
                 0,
                 gl_array_internal_format,
                 width,
                 height,
                 num_pngs,
                 0,
                 gl_array_format,
                 gl_array_type,
                 NULL);

    // populate the new array texture
    for (int i = 0; i < num_pngs; i++)
    {
        const struct png_t *png = &pngs[i];

        // get the opengl representations of the current textures properties
        enum texture_format_t png_format = texture_format_from_png(png->format);
        GLenum gl_png_internal_format, gl_png_format, gl_png_type;
        texture_format_to_gl(png_format, &gl_png_internal_format, &gl_png_format, &gl_png_type);

        // populate the current element in the array texture with the current pngs texture
        glTexSubImage3D(gl_target,
                        0,
                        0,
                        0,
                        i,
                        png->width,
                        png->height,
                        1,
                        gl_png_format,
                        gl_png_type,
                        png->data);
    }

    // generate the mipmap now that the array texture is populated
    glGenerateMipmap(gl_target);

    // initialize the given texture
    texture->width = width;
    texture->height = height;
    texture->type = type;
    texture->scaling = scaling;
    texture->format = array_format;
    texture->id = id;
}

void texture_init_empty(struct texture_t *texture,
                        unsigned int width,
                        unsigned int height,
                        enum texture_scaling_t scaling,
                        enum texture_format_t format)
{
    // get the opengl representations of the new textures properties
    enum texture_type_t type = TEXTURE_2D;
    GLenum gl_target, gl_internal_format, gl_format, gl_type;
    texture_type_to_gl(type, &gl_target);
    texture_format_to_gl(format, &gl_internal_format, &gl_format, &gl_type);

    // create and populate the new texture
    GLuint id = texture_create(type, scaling);
    glTexImage2D(gl_target,
                 0,
                 gl_internal_format,
                 width,
                 height,
                 0,
                 gl_format,
                 gl_type,
                 NULL);

    // initialize the given texture
    texture->width = width;
    texture->height = height;
    texture->type = type;
    texture->scaling = scaling;
    texture->format = format;
    texture->id = id;
}

void texture_deinit(struct texture_t *texture)
{
    glDeleteTextures(1, &texture->id);
}

void texture_bind(const struct texture_t *texture, unsigned int unit)
{
    // ensure the given unit it valid
    assert(unit < TEXTURE_MAX_UNITS);

    // get the opengl target for the given textures type
    GLenum gl_target;
    texture_type_to_gl(texture->type, &gl_target);

    // activate and bind to the given unit
    texture_activate_unit(unit);
    glBindTexture(gl_target, texture->id);
}
