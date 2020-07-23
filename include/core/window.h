#pragma once

#include <stdbool.h>

#include "gl.h"
#include "colour.h"

///
/// Windows are used to manage independent desktop windows which contain graphics contexts.
///

// MARK: - Data Structures

/// A window on the desktop containing a graphics context.
struct window_t
{
    /// The backing GLFW window of this window.
    GLFWwindow *backing;
};

// MARK: - Functions

/// Initialize the given window with the given parameters.
///
/// The new window will not be resizable.
/// The new window always contains an OpenGL 3.3 core graphics context.
/// During this function the new window is set as the current window for the calling thread.
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

/// Set the given window's graphics context to be current.
///
/// This must be called before any graphics-related functions can be used within the given window.
/// Only one window at a time can be current on a given thread, but several windows can be current across separate threads.
/// @param window The window containing the graphics context to make current.
void window_set_current(struct window_t *window);

/// Set the background colour of the given window to the given colour.
/// @param window The window to set the background colour of.
/// @param colour The colour to set the background colour to.
void window_set_background(struct window_t *window, const struct colour_t *colour);

/// Get whether or not the given window was closed by the user.
///
/// Window closure is not automatic, this only indicates whether or not the user tried to close the given window.
/// This should be checked before beginning a new frame to determine whether or not the given window's frame loop should terminate.
/// `core_poll_events(struct core_t *)` must be called before this value will update.
/// @param window The window to check.
/// @return Whether or not the given window was closed by the user.
bool window_is_closed(struct window_t *window);

/// Begin a new frame that will be displayed by the given window.
///
/// This must be called at the beginning of every frame for the given window.
/// @param window The window to begin the new frame in.
void window_begin_frame(struct window_t *window);

/// End the current frame and display it within the given window.
///
/// This must be called at the end of every frame for the given window.
/// Once the frame is displayed then this function waits for vertical sync.
/// @param window The window to display the new frame within.
void window_end_frame(struct window_t *window);
