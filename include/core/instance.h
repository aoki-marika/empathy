#pragma once

#include "window.h"
#include "framebuffer.h"

///
/// Instances are used to handle the lifetime and final rendering of a single instance of a program which uses core.
///
/// The programs that run within instances are called "programs".
/// Instances handle the frame loop and displaying the final framebuffer of their program,
/// but the creator is responsible for implementing the program and global event polling.
/// Each instance and it's program is initialized and run on it's own thread, so multiple can coexist at the same time across several windows,
/// but the creator is responsible for managing the main thread's global event polling and termination.
///
/// Instances have multiple "outputs", which define how the final framebuffer is displayed within the window:
///  - Fullscreen: The final framebuffer is drawn within the full bounds of the window, with no additional components.
///  - Debug: The final framebuffer is drawn within an IMGUI window, with various debugging tools displayed around it.
///

// MARK: - Type Definitions

/// A function which is used to initialize an instance's program or output.
/// @param data The user data pointer supplied to the calling instance.
typedef void (* instance_init_function_t)(void *data);

/// A function which is used to deinitialize an instance's program or output.
/// @param data The user data pointer supplied to the calling instance.
typedef void (* instance_deinit_function_t)(void *data);

/// A function which is used to render a single frame of an instance's program.
///
/// The given framebuffer is already set as the render target when this function is called,
/// so if there is no intermediate framebuffer then it does not have to be set before rendering.
/// @param data The user data pointer supplied to the calling instance.
/// @param framebuffer The final framebuffer to render the new frame to.
/// This is the framebuffer that is rendered to the screen, so if this framebuffer is not rendered to then nothing is displayed.
typedef void (* instance_program_render_function_t)(void *data, struct framebuffer_t *framebuffer);

/// A function which is used to render the final framebuffer of an instance's program to the screen.
///
/// The final pass has already begun when this function is called.
/// @param data The user data pointer supplied to the calling instance.
/// @param framebuffer The final framebuffer to draw.
typedef void (* instance_output_render_function_t)(void *data, struct framebuffer_t *framebuffer);

// MARK: - Data Structures

/// A single core program instance.
struct instance_t
{
    /// The window which this instance renders in.
    struct window_t *window;

    /// The final framebuffer that this instance renders to to be displayed.
    struct framebuffer_t framebuffer;

    /// The output that this instance is using.
    struct instance_output_t
    {
        /// The user data pointer to supply to this output's functions.
        void *data;

        /// The function used to initialize this output.
        instance_init_function_t init;

        /// The function used to deinitialize this output.
        instance_deinit_function_t deinit;

        /// The function used to render a single frame of this output to the screen.
        instance_program_render_function_t render;
    } output;

    /// The program of this instance.
    struct instance_program_t
    {
        /// The user data pointer to supply to this program's functions.
        void *data;

        /// The function used to initialize this program.
        instance_init_function_t init;

        /// The function used to deinitialize this program.
        instance_deinit_function_t deinit;

        /// The function used to render a single frame of this program.
        instance_program_render_function_t render;

        /// Whether or not this program is currently running.
        bool is_running;
    } program;
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
/// @param data The user data pointer to supply to the new instance's program's functions.
/// This pointer must be able to be accessed from different threads.
/// @param init The function to call to initialize the new instance's program.
/// See `instance_init_function_t` for further documentation.
/// @param deinit The function to call to deinitialize the new instance's program.
/// See `instance_deinit_function_t` for further documentation.
/// @param render The function to call to render a single frame of the new instance's program.
/// See `instance_program_render_function_t` for further documentation.
void instance_init(struct instance_t *instance,
                   struct window_t *window,
                   unsigned int render_width,
                   unsigned int render_height,
                   void *data,
                   instance_init_function_t init,
                   instance_deinit_function_t deinit,
                   instance_program_render_function_t render);

/// Deinitialize the given instance, releasing all of it's allocated resources.
///
/// If the given instance's program is currently running then the program terminates.
/// @param instance The instance to deinitialize.
void instance_deinit(struct instance_t *instance);

/// Get whether or not the given instance's program is currently running.
/// @param instance The instance to check the program of.
/// @return Whether or not the given instance's program is currently running.
bool instance_is_running(struct instance_t *instance);

/// Create a new thread and run the given instance's program within it.
///
/// Once the given instance's program is run it cannot be stopped,
/// unless the user closes the given instance's window or the program is terminated.
/// It is expected that the caller manages global event polling,
/// if it does not then the given instance's window cannot be closed and will behave unexpectedly.
/// @param instance The instance to run the program of.
void instance_run(struct instance_t *instance);
