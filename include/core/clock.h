#pragma once

///
/// Clocks are used to track system time with high accuracy, in a self-contained relative time.
///

// MARK: - Data Structures

/// A clock for tracking system time at runtime.
struct clock_t
{
    /// The time, in milliseconds, that this clock uses as its reference point.
    double start_time;
};

// MARK: - Functions

/// Initialize the given clock, setting its start time to the current time.
/// @param clock The clock to initialize.
void clock_init(struct clock_t *clock);

/// Deinitialize the given clock, releasing all of its allocated resources.
/// @param clock The clock to deinitialize.
void clock_deinit(struct clock_t *clock);

/// Get the current relative time of the given clock.
/// @param clock The clock to get the time of.
/// @return The current relative time of the given clock, in milliseconds.
double clock_get_time(struct clock_t *clock);

/// Reset the start time of the given clock to the current time.
/// @param clock The clock to reset.
void clock_reset(struct clock_t *clock);
