#pragma once

#include <stdbool.h>

///
/// Core contexts are the global state of the core engine, which most subsystems rely on.
///
/// Core contexts are not thread safe, and functions related to them should only be called on the main thread.
///

// MARK: - Data Structures

/// The state of a core engine context.
struct core_t
{
};

// MARK: - Functions

/// Initialize the given core context.
///
/// A program must initialize a single core context before using any part of the engine.
/// Only one core context is intended to be created per-program, any more is undefined behaviour.
/// @param core The core context to initialize.
void core_init(struct core_t *core);

/// Deinitialize the given core context, releasing all of its allocated resources.
/// @param core The core context to deinitialize.
void core_deinit(struct core_t *core);

/// Poll for new global events within the given core context.
///
/// This should be called at the beginning of each frame, before updating anything else.
/// @param core The core context to poll for global events.
/// @param wait Whether or not the current thread should sleep until new global events are queued.
void core_poll_events(struct core_t *core, bool wait);
