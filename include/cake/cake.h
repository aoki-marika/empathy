#pragma once

#include <core/instance.h>

///
/// Types and functions related to a cake instance and its program.
///

// MARK: - Macros

/// The width of a window containing a cake instance, in pixels.
#define CAKE_WINDOW_WIDTH 1600

/// The height of a window containing a cake instance, in pixels.
#define CAKE_WINDOW_HEIGHT 900

/// The width at which a cake instance's program renders at, in pixels.
#define CAKE_RENDER_WIDTH 1920

/// The height at which a cake instance's program renders at, in pixels.
#define CAKE_RENDER_HEIGHT 1080

// MARK: - Data Structures

/// A single cake context used by an instance.
struct cake_t
{
};

// MARK: - Functions

/// The instance program initializer function for a cake instance.
///
/// It assumed that the given data is a cake context.
/// See `instance_init_function_t` for further documentation.
void cake_init(struct instance_t *instance,
               void *data);

/// The instance program deinitializer function for a cake instance.
///
/// It assumed that the given data is a cake context.
/// See `instance_deinit_function_t` for further documentation.
void cake_deinit(struct instance_t *instance,
                 void *data);

/// The instance program render function for a cake instance.
///
/// It assumed that the given data is a cake context.
/// See `instance_program_render_function_t` for further documentation.
void cake_render(struct instance_t *instance,
                 void *data,
                 struct framebuffer_t *framebuffer);
