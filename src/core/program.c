#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MARK: - Functions

/// Attempt to locate the given named uniform within the given program, and return the result.
/// @param program The program to locate the uniform within.
/// @param name The name of the uniform to locate.
/// @return The unique OpenGL identifier of the given uniform's location within the given program.
/// If the result is less than zero then the uniform could not be located.
GLint program_locate_uniform(struct program_t *program, const char *name)
{
    // check the cache to see if the given uniform was already located
    for (int i = 0; i < program->num_cached_uniforms; i++)
    {
        struct program_uniform_t *uniform = &program->cached_uniforms[i];
        if (strcmp(uniform->name, name) == 0)
            return uniform->location;
    }

    // get the location, returning early if there was an error
    GLint location = glGetUniformLocation(program->id, name);
    if (location < 0)
        return location;

    // ensure the cache isnt full, if it is then clear it
    if (program->num_cached_uniforms + 1 > PROGRAM_MAX_CACHED_UNIFORMS)
    {
        for (int i = 0; i < program->num_cached_uniforms; i++)
            free(program->cached_uniforms[i].name);

        program->num_cached_uniforms = 0;
    }

    // cache the location
    // the string needs to be copied to an allocated region first to allow access later
    // strlen is incremented to account for the terminator character
    size_t name_size = strlen(name) + 1;
    char *name_copy = (char *)malloc(name_size * sizeof(char));
    strcpy(name_copy, name);

    struct program_uniform_t uniform;
    uniform.name = name_copy;
    uniform.location = location;
    program->cached_uniforms[program->num_cached_uniforms++] = uniform;

    // return the uniforms location
    return location;
}

void program_init(struct program_t *program,
                  unsigned int num_shaders,
                  const struct shader_t *shaders)
{
    // create the program and link the shaders
    GLuint id = glCreateProgram();
    for (int i = 0; i < num_shaders; i++)
        glAttachShader(id, shaders[i].id);
    glLinkProgram(id);

    // check for linker errors
    GLint is_linked;
    glGetProgramiv(id, GL_LINK_STATUS, &is_linked);
    if (is_linked != GL_TRUE)
    {
        // get the log
        int log_length;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);

        char log[log_length];
        glGetProgramInfoLog(id, log_length, &log_length, log);

        // print the log and terminate
        fprintf(stderr, "PROGRAM ERROR: %s\n", log);
        exit(EXIT_FAILURE);
    }

    // initialize the program
    program->id = id;
    program->num_cached_uniforms = 0;
}

void program_deinit(struct program_t *program)
{
    for (int i = 0; i < program->num_cached_uniforms; i++)
        free(program->cached_uniforms[i].name);

    glDeleteProgram(program->id);
}

void program_use(struct program_t *program)
{
    glUseProgram(program->id);
}

void program_set_sampler2d(struct program_t *program,
                           const char *name,
                           const struct texture_t *texture)
{
    // the program needs to be in use to set uniforms
    // just to keep consistency its set regardless of whether or not the uniform is located
    program_use(program);

    // locate the uniform, returning early if it cannot be located
    GLint location = program_locate_uniform(program, name);
    if (location < 0)
        return;

    // set the uniform to the given textures unit
    // sampler2d uniforms use unit indices instead of enum cases
    glUniform1i(location, texture->unit - GL_TEXTURE0);
}
