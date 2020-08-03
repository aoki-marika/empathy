#pragma once

#include <stdbool.h>

///
/// Clocks are used to track system time with high accuracy, in a self-contained relative time.
///

// MARK: - Data Structures

/// A clock for tracking system time at runtime.
struct clock_t
{
    /// The time, in milliseconds, that this clock uses as its reference point.
    double reference_time;

    /// Whether or not this clock is currently paused.
    bool is_paused;

    /// The last time, in milliseconds, at which this clock was paused, if any.
    double pause_time;
};

// MARK: - Functions

/// Initialize the given clock, setting its reference time to the current time.
/// @param clock The clock to initialize.
void clock_init(struct clock_t *clock);

/// Deinitialize the given clock, releasing all of its allocated resources.
/// @param clock The clock to deinitialize.
void clock_deinit(struct clock_t *clock);

/// Get the current relative time of the given clock.
/// @param clock The clock to get the time of.
/// @return The current relative time of the given clock, in milliseconds.
double clock_get_time(struct clock_t *clock);

/// Set whether or not the given clock is paused.
///
/// While a clock is paused its relative time is frozen,
/// then when it is unpaused the relative time resumes from where it was when it was paused.
/// If the pause state of the given clock matches the given value then this function does nothing.
/// @param clock The clock to set the pause state of.
/// @param is_paused Whether the given clock should be paused or unpaused.
void clock_set_paused(struct clock_t *clock, bool is_paused);

/// Reset the reference time of the given clock to the current time.
///
/// If the given clock is paused then it is automatically unpaused.
/// @param clock The clock to reset.
void clock_reset(struct clock_t *clock);
