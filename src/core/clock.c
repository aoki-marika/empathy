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
    // initialize the given clock
    clock->is_paused = false;

    // set the initial reference time
    clock_reset(clock);
}

void clock_deinit(struct clock_t *clock)
{
}

double clock_get_time(struct clock_t *clock)
{
    // return the relative paused time if the given clock is paused
    // return the relative time if the given clock is unpaused
    if (clock->is_paused)
        return clock->pause_time - clock->reference_time;
    else
        return clock_get_absolute_time() - clock->reference_time;
}

void clock_set_paused(struct clock_t *clock, bool is_paused)
{
    // if the given pause state is already set then there is nothing to do
    if (is_paused == clock->is_paused)
        return;

    // when the clock is paused store the time so it can be used as the reference time
    // when the clock is unpaused change the reference time to include the relative time before being paused
    if (is_paused)
        clock->pause_time = clock_get_absolute_time();
    else
        clock->reference_time = clock_get_absolute_time() - (clock->pause_time - clock->reference_time);

    clock->is_paused = is_paused;
}

void clock_reset(struct clock_t *clock)
{
    // ensure the clock is unpaused
    if (clock->is_paused)
        clock_set_paused(clock, false);

    clock->reference_time = clock_get_absolute_time();
}
