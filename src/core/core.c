#include "core.h"

#include <stdio.h>
#include <stdlib.h>

#include "gl.h"

// MARK: - Functions

/// Throw the given core context error, terminating the program.
/// @param error The integer code of the error.
/// @param description A human readable description of the error.
void core_throw_error(int error, const char *description)
{
    // print the error and exit
    fprintf(stderr, "CORE ERROR: %s (0x%08x, %i)\n", description, error, error);
    exit(EXIT_FAILURE);
}

void core_init(struct core_t *core)
{
    // init glfw
    if (!glfwInit())
    {
        // init failure can only produce one error, with a static description
        // there is no way to retrieve this error so describe it manually
        int error = GLFW_PLATFORM_ERROR;
        const char *description = "A platform-specific error occurred that does not match any of the more specific categories.";
        core_throw_error(error, description);
    }

    // throw any glfw errors
    glfwSetErrorCallback(core_throw_error);
}

void core_deinit(struct core_t *core)
{
    glfwTerminate();
}

void core_update(struct core_t *core, bool wait_for_event)
{
    if (wait_for_event)
        glfwWaitEvents();
    else
        glfwPollEvents();
}
