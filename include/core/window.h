#pragma once

#include <stdbool.h>

#include "gl.h"
#include "colour.h"
#include "texture.h"

///
/// Windows are independent desktop windows which contain a graphics context and render to a framebuffer texture.
///
/// Windows on their own do not display anything, it is entirely up to the creator to manage the frame loop and display the framebuffer.
/// The general frame loop should look similar to this:
///  - Check if the window is still open with `window_is_closed(struct window_t *)`.
///     - If it is closed then terminate the frame loop.
///  - Begin the new frame with `window_begin_frame(struct window_t *)`.
///     - This indicates to the graphics context to begin a new frame, clearing out any previous state.
///  - Draw the frame.
///  - Begin drawing the framebuffer texture with `window_begin_framebuffer_draw(struct window_t *)`.
///     - This configures the graphics context to stop rendering to the framebuffer texture, and instead to the screen.
///  - Draw the framebuffer texture, `window_t.framebuffer_texture`.
///     - This is entirely up to the caller to manage, as to allow more flexibility in cases like
///       debug sessions where the framebuffer is drawn within a special interface.
///  - End the new frame with `window_end_frame(struct window_t *)`.
///     - This pushes all the new state of the frame to the graphics context, and waits for the appropriate frame interval.
///

// MARK: - Data Structures

/// A window on the desktop containing a graphics context.
struct window_t
{
    /// The backing GLFW window of this window.
    GLFWwindow *backing;

    /// The unique OpenGL identifier of this window's framebuffer
    GLuint framebuffer_id;

    /// The backing texture of this window's framebuffer.
    struct texture_t framebuffer_texture;
};

// MARK: - Functions

/// Initialize the given window with the given parameters.
///
/// The new window always contains an OpenGL 3.3 core graphics context.
/// During this function the new window is set as the current window for the calling thread.
/// @param window The window to initialize.
/// @param width The width of the new window, in pixels.
/// @param height The height of the new window, in pixels.
/// @param title The title of the new window.
/// @param resizable Whether or not the new window should be resizable.
/// Although the window can be resized, the framebuffer will always be the same given resolution,
/// and the viewport will not be changed.
void window_init(struct window_t *window,
                 unsigned int width,
                 unsigned int height,
                 const char *title,
                 bool is_resizable);

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
///
/// During this function the given window is set as the current window for the calling thread.
/// @param window The window to set the background colour of.
/// @param colour The colour to set the background colour to.
void window_set_background(struct window_t *window, struct colour4_t colour);

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
/// During this function the given window is set as the current window for the calling thread.
/// @param window The window to begin the new frame in.
void window_begin_frame(struct window_t *window);

/// Indicate to the given window that the rest of the frame is dedicated to drawing the framebuffer.
///
/// If this is not called during a frame then the entire frame will be drawn to the framebuffer, and nothing will be displayed.
/// It is assumed that the given window is already current for the calling thread.
/// @param window The window to begin drawing the framebuffer in.
void window_begin_framebuffer_draw(struct window_t *window);

/// End the current frame and display it within the given window.
///
/// This must be called at the end of every frame for the given window.
/// Once the frame is displayed then this function waits for vertical sync.
/// It is assumed that the given window is already current for the calling thread.
/// @param window The window to display the new frame within.
void window_end_frame(struct window_t *window);
