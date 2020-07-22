#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

// MARK: - Functions

void shader_init(struct shader_t *shader, enum shader_type_t type, const char *source)
{
    // map the given type to its opengl representation
    GLenum gl_type;
    switch (type)
    {
        case SHADER_VERTEX:   gl_type = GL_VERTEX_SHADER; break;
        case SHADER_FRAGMENT: gl_type = GL_FRAGMENT_SHADER; break;
        case SHADER_GEOMETRY: gl_type = GL_GEOMETRY_SHADER; break;
    }

    // create and compile the shader
    GLuint id = glCreateShader(gl_type);
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);

    // check for compilation errors
    GLint is_compiled;
    glGetShaderiv(id, GL_COMPILE_STATUS, &is_compiled);
    if (is_compiled != GL_TRUE)
    {
        // get the log
        int log_length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);

        char log[log_length];
        glGetShaderInfoLog(id, log_length, &log_length, log);

        // print the log and terminate
        // include the source for easier analysis
        fprintf(stderr, "SHADER ERROR: %s\n%s\n", log, source);
        exit(EXIT_FAILURE);
    }

    // initialize the shader
    shader->id = id;
}

void shader_deinit(struct shader_t *shader)
{
    glDeleteShader(shader->id);
}
