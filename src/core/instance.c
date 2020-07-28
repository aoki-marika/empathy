#include "instance.h"

#include <stdlib.h>
#include <pthread.h>

// MARK: - Functions

/// Run the given instance's program on the current thread.
///
/// This is the actual function which runs the given instance's program,
/// `instance_run(struct instance_t *)` is only a wrapper to create the thread.
/// @param arguemnt The pointer to the instance to run the program of.
void *instance_run_internal(void *argument)
{
    struct instance_t *instance = (struct instance_t *)argument;

    // set the given window as current on this new thread
    window_set_current(instance->window);

    // initialize the given instances output and program
    instance->output.init(instance, instance->output.data);
    instance->program.init(instance, instance->program.data);

    // run the frame loop
    while (!window_is_closed(instance->window))
    {
        // begin the frame
        window_begin_frame(instance->window);

        // render the program frame
        framebuffer_use(&instance->framebuffer);
        instance->program.render(instance, instance->program.data, &instance->framebuffer);

        // render the output to the screen
        window_begin_final_pass(instance->window);
        instance->output.render(instance, instance->output.data, &instance->framebuffer);

        // draw the frame
        window_end_frame(instance->window);
    }

    // deinitialize the given instances output and program
    instance->program.deinit(instance, instance->program.data);
    instance->output.deinit(instance, instance->output.data);

    // mark the given instances program as finished
    instance->program.is_running = false;
    return NULL;
}

void instance_init(struct instance_t *instance,
                   struct window_t *window,
                   unsigned int render_width,
                   unsigned int render_height,
                   void *data,
                   instance_init_function_t init,
                   instance_deinit_function_t deinit,
                   instance_program_render_function_t render,
                   struct instance_output_t output)
{
    // set the given window as current to ensure everything is created in the correct context
    window_set_current(window);

    // initialize the given instance
    instance->window = window;
    framebuffer_init(&instance->framebuffer, render_width, render_height);

    // initialize the given instances program
    instance->program.data = data;
    instance->program.init = init;
    instance->program.deinit = deinit;
    instance->program.render = render;
    instance->program.is_running = false;

    // initialize the given instances output
    instance->output = output;
}

void instance_deinit(struct instance_t *instance)
{
    // ensure the given instances program isnt running
    // there would be unexpected results if the program is running and the instance is deinitialized
    if (instance_is_running(instance))
    {
        // the given instances program is running, print the details and terminate
        fprintf(stderr, "INSTANCE ERROR: tried to deinitialize instance %p of running program\n", instance);
        exit(EXIT_FAILURE);
    }

    framebuffer_deinit(&instance->framebuffer);
}

bool instance_is_running(struct instance_t *instance)
{
    return instance->program.is_running;
}

void instance_run(struct instance_t *instance)
{
    // mark the given instances program as running
    // done here instead of the thread to avoid a race condition where the state is checked immediately after running
    instance->program.is_running = true;

    // create the thread and actually run the given instances program
    pthread_t thread;
    pthread_create(&thread, NULL, instance_run_internal, instance);
}
