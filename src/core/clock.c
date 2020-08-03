#include "clock.h"

#include "gl.h"

// MARK: - Functions

/// Get the current absolute time.
/// @return The current absolute time, in milliseconds.
double clock_get_absolute_time()
{
    // glfw measures time in seconds, so convert to milliseconds
    return glfwGetTime() * 1000;
}

void clock_init(struct clock_t *clock)
{
    clock_reset(clock);
}

void clock_deinit(struct clock_t *clock)
{
}

double clock_get_time(struct clock_t *clock)
{
    return clock_get_absolute_time() - clock->start_time;
}

void clock_reset(struct clock_t *clock)
{
    clock->start_time = clock_get_absolute_time();
}
