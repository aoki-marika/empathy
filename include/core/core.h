#pragma once

///
/// Core contexts are used to manage global core engine state.
/// This is mostly used for initializing global C libraries like GLFW, which is shared across several other objects.
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
/// This core context must then be deinitialized once engine usage is complete.
/// @param core The core context to initialize.
void core_init(struct core_t *core);

/// Deinitialize the given core context, releasing all of it's allocated resources.
/// @param core The core context to deinitialize.
void core_deinit(struct core_t *core);
