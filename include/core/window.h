#pragma once

#include <stdbool.h>

#include "gl.h"

///
/// Windows are used to manage independent desktop windows which contain OpenGL contexts.
///

// MARK: - Data Structures

/// A window on the desktop containing an OpenGL context.
struct window_t
{
    /// The backing GLFW window of this window.
    GLFWwindow *backing;
};

// MARK: - Functions

/// Initialize the given window with the given parameters.
///
/// The new window will not be resizable, and always contains an OpenGL 3.3 core context.
/// @param window The window to initialize.
/// @param width The width of the new window, in pixels.
/// @param height The height of the new window, in pixels.
/// @param title The title of the new window.
void window_init(struct window_t *window,
                 unsigned int width,
                 unsigned int height,
                 const char *title);

/// Deinitialize the given window, releasing all of it's allocated resources.
/// @param window The window to deinitialize.
void window_deinit(struct window_t *window);

/// Get whether or not the given window should be closed.
///
/// This should be checked at the beginning of each frame of the given window to determine whether or not the window's frame loop should terminate.
/// `core_poll_events(struct core_t *)` must be called before this value will update.
/// @param window The window to check.
/// @return Whether or not the given window should be closed.
bool window_should_close(struct window_t *window);

/// Make the given window's OpenGL context current.
///
/// This must be called before OpenGL can be used with the given window, and must be called again when it is called on another window.
/// @param window The window containing the OpenGL context to make current.
void window_make_current(struct window_t *window);

/// Swap the double framebuffers of the given window, displaying the newly drawn one, and wait for vertical sync.
///
/// This should be called at the end of each frame of the given window, after drawing into the framebuffer.
/// @param window The window to swap the framebuffers of.
void window_swap_buffers(struct window_t *window);
