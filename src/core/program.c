#include "program.h"

#include <stdio.h>
#include <stdlib.h>

// MARK: - Functions

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
}

void program_deinit(struct program_t *program)
{
    glDeleteProgram(program->id);
}

void program_use(struct program_t *program)
{
    glUseProgram(program->id);
}
