#pragma once

#include <stdbool.h>

#include "gl.h"
#include "colour.h"
#include "texture.h"

///
/// Windows are independent desktop windows which contain a graphics context.
///
/// Windows are designed to be very generic so that the frame loop can be handled by the creator.
/// The general frame loop should look similar to this:
///  - Check if the window is still open with `window_is_closed(struct window_t *)`.
///     - If it is closed then terminate the frame loop.
///  - Begin the new frame with `window_begin_frame(struct window_t *)`.
///     - This indicates to the graphics context to begin a new frame, clearing out any previous state.
///  - Poll the core context for global events.
///  - If not using framebuffers, begin the final render pass with `window_begin_final_pass(struct window_t *)`.
///  - Draw the frame.
//   - If using framebuffers, begin the final render pass with `window_begin_final_pass(struct window_t *)`, and draw the framebuffer
///  - End the new frame with `window_end_frame(struct window_t *)`.
///     - This pushes all the new state of the frame to the graphics context, and waits for the appropriate frame interval.
///

// MARK: - Data Structures

/// A window on the desktop containing a graphics context.
struct window_t
{
    /// The width of this window, in pixels.
    unsigned int width;

    /// The height of this window, in pixels.
    unsigned int height;

    /// The backing GLFW window of this window.
    GLFWwindow *backing;
};

// MARK: - Functions

/// Initialize the given window with the given parameters.
///
/// The new window always contains an OpenGL 3.3 core graphics context.
/// During this function the current graphics context of the calling thread is cleared.
/// @param window The window to initialize.
/// @param width The width of the new window, in pixels.
/// @param height The height of the new window, in pixels.
/// @param title The title of the new window.
/// @param resizable Whether or not the new window should be resizable.
/// Although the window can be resized, the viewport is never changed.
void window_init(struct window_t *window,
                 unsigned int width,
                 unsigned int height,
                 const char *title,
                 bool is_resizable);

/// Deinitialize the given window, releasing all of its allocated resources.
/// @param window The window to deinitialize.
void window_deinit(struct window_t *window);

/// Set the given window's graphics context to be current.
///
/// This must be called before any graphics-related functions can be used within the given window.
/// Only one window at a time can be current on a given thread, but several windows can be current across separate threads.
/// @param window The window containing the graphics context to make current.
void window_set_current(struct window_t *window);

/// Clear the current graphics context on the calling thread.
///
/// This must be called before the current graphics context on the calling thread can be set on another thread.
void window_clear_current();

/// Set the background colour of the given window to the given colour.
///
/// During this function the given window is set as the current window for the calling thread.
/// @param window The window to set the background colour of.
/// @param colour The colour to set the background colour to.
void window_set_background(struct window_t *window, struct colour4_t colour);

/// Get whether or not the given window wants to be closed.
///
/// Window closure is not automatic, this only indicates whether or not the window wants to be closed.
/// See `window_deinit(struct window_t *)` for closing the window.
/// `core_update(struct core_t *)` must be called before this value will update in response to user input.
/// @param window The window to get the closed state of.
/// @return Whether or not the given window wants to close.
bool window_is_closed(struct window_t *window);

/// Set whether or not the given window wants to be closed.
/// @param window The window to set the closed state of.
/// @param is_closed Whether or not the window should be marked as wanting to be closed.
void window_set_closed(struct window_t *window, bool is_closed);

/// Begin a new frame that will be displayed by the given window.
///
/// This must be called at the beginning of every frame for the given window.
/// During this function the given window is set as the current window for the calling thread.
/// @param window The window to begin the new frame in.
void window_begin_frame(struct window_t *window);

/// Begin the final render pass of the given window.
///
/// This indicates to the graphics context to render to the screen instead of a framebuffer.
/// During this function the given window's viewport is applied to the current graphics context.
/// It is assumed that the given window is already current for the calling thread.
/// @param window The window to begin the final render pass on.
void window_begin_final_pass(struct window_t *window);

/// End the current frame and display it within the given window.
///
/// This must be called at the end of every frame for the given window.
/// Once the frame is displayed then this function waits for vertical sync.
/// It is assumed that the given window is already current for the calling thread.
/// @param window The window to display the new frame within.
void window_end_frame(struct window_t *window);
