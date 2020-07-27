#include "instance.h"

#include <pthread.h>

// MARK: - Functions

/// Run the given instance on the current thread.
///
/// This is the actual function which runs the instance,
/// `instance_run(struct instance_t *)` is only a wrapper to create the thread.
/// @param arguemnt The pointer to the instance to run.
void *instance_run_internal(void *argument)
{
    struct instance_t *instance = (struct instance_t *)argument;

    // set the given window as current on this new thread
    window_set_current(instance->window);

    // initialize the given instances program
    instance->init(instance->data);

    // run the frame loop
    while (!window_is_closed(instance->window))
    {
        // begin the frame
        window_begin_frame(instance->window);

        // render the program frame
        framebuffer_use(&instance->framebuffer);
        instance->render(instance->data, &instance->framebuffer);

        // render the final framebuffer
        // TODO: render the final framebuffer

        // draw the frame
        window_end_frame(instance->window);
    }

    // deinitialize the given instances program
    instance->deinit(instance->data);
    return NULL;
}

void instance_init(struct instance_t *instance,
                   struct window_t *window,
                   unsigned int render_width,
                   unsigned int render_height,
                   enum instance_display_mode_t display_mode,
                   void *data,
                   instance_init_function_t init,
                   instance_deinit_function_t deinit,
                   instance_render_function_t render)
{
    // set the given window as current to ensure everything is created in the correct context
    window_set_current(window);

    // initialize the given instance
    instance->window = window;
    framebuffer_init(&instance->framebuffer, render_width, render_height);
    instance->display_mode = display_mode;
    instance->data = data;
    instance->init = init;
    instance->deinit = deinit;
    instance->render = render;
}

void instance_deinit(struct instance_t *instance)
{
    framebuffer_deinit(&instance->framebuffer);
}

void instance_run(struct instance_t *instance)
{
    // create the thread and actually run the given instance
    pthread_t thread;
    pthread_create(&thread, NULL, instance_run_internal, instance);
}
