#pragma once

#include "window.h"
#include "framebuffer.h"

///
/// Instances are used to handle the lifetime and final rendering of a single instance of a program which uses core.
///
/// Instances handle the frame loop and displaying the final framebuffer,
/// but the creator is responsible for implementing the program and global event polling.
/// Each instance is initialized and run on it's own thread, so multiple can coexist at the same time across several windows,
/// but the creator is responsible for managing the main thread's global event polling and termination.
///
/// Instances have multiple "display modes", which define how the final framebuffer is displayed within the window.
///  - Fullscreen: The final framebuffer is drawn within the full bounds of the window, with no additional components.
///  - Debug: The final framebuffer is drawn within an IMGUI window, with various debugging tools displayed around it.
///

// MARK: - Type Definitions

/// A function which is used to initialize an instance's program.
/// @param data The user data pointer supplied to the calling instance.
typedef void (* instance_init_function_t)(void *data);

/// A function which is used to deinitialize an instance's program.
/// @param data The user data pointer supplied to the calling instance.
typedef void (* instance_deinit_function_t)(void *data);

/// A function which is used to render a single frame of an instance's program.
///
/// The given framebuffer is already set as the render target when this function is called,
/// so if there is no intermediate framebuffer then it does not have to be set.
/// @param data The user data pointer supplied to the calling instance.
/// @param framebuffer The final framebuffer to render the new frame to.
/// This is the framebuffer that is rendered to the screen, so if this framebuffer is not rendered to then nothing is displayed.
typedef void (* instance_render_function_t)(void *data, struct framebuffer_t *framebuffer);

// MARK: - Data Structures

/// A single core program instance.
struct instance_t
{
    /// The window which this instance renders in.
    struct window_t *window;

    /// The final framebuffer that this instance renders to to be displayed.
    struct framebuffer_t framebuffer;

    /// The display mode of this instance.
    enum instance_display_mode_t
    {
        /// Fill the window with the final framebuffer.
        INSTANCE_FULLSCREEN = 0,

        /// Draw the final framebuffer in an IMGUI window, with debugging tools.
        INSTANCE_DEBUG = 1,
    } display_mode;

    /// The user data pointer to supply to this instance's program's functions.
    void *data;

    /// The function used to initialize this instance's program.
    instance_init_function_t init;

    /// The function used to deinitialize this instance's program.
    instance_deinit_function_t deinit;

    /// The function used to render a single frame of this instance's program.
    instance_render_function_t render;

    /// Whether or not this instance is currently running.
    bool is_running;
};

// MARK: - Functions

/// Initialize the given instance with the given render size.
///
/// During this function the given window's graphics context is set as current.
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// During this function the current render target is reset.
/// @param instance The instance to deinitialize.
/// @param render_width The width, in pixels, for the new instance to render at.
/// @param render_height The height, in pixels, for the new instance to render at.
/// @param window The window for the new instance to render to.
/// It is expected that this window is available for the entire lifetime of the given instance.
/// @param display_mode The display mode for thew instance to use.
/// @param data The user data pointer to supply to the new instance's program's functions.
/// This pointer must be able to be accessed from different threads.
/// @param init The function to call to initialize the new instance's program.
/// See `instance_init_function_t` for further documentation.
/// @param deinit The function to call to deinitialize the new instance's program.
/// See `instance_deinit_function_t` for further documentation.
/// @param render The function to call to render a single frame of the new instance's program.
/// See `instance_render_function_t` for further documentation.
void instance_init(struct instance_t *instance,
                   struct window_t *window,
                   unsigned int render_width,
                   unsigned int render_height,
                   enum instance_display_mode_t display_mode,
                   void *data,
                   instance_init_function_t init,
                   instance_deinit_function_t deinit,
                   instance_render_function_t render);

/// Deinitialize the given instance, releasing all of it's allocated resources.
///
/// If the given instance is currently running then the program terminates.
/// @param instance The instance to deinitialize.
void instance_deinit(struct instance_t *instance);

/// Create a new thread and run the given instance within it.
///
/// Once the given instance is run it cannot be stopped,
/// unless the user closes the given instance's window or the program is terminated.
/// It is expected that the caller manages global event polling,
/// if it does not then the given instance's window cannot be closed and will behave unexpectedly.
/// @param instance The instance to run.
void instance_run(struct instance_t *instance);
