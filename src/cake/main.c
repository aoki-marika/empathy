#include <stdlib.h>
#include <core/core.h>
#include <core/window.h>
#include <core/instance.h>
#include <core/instance_output_imgui.h>

#include "cake.h"

// MARK: - Functions

int main(int argc, char **argv)
{
    // create the core context
    struct core_t core;
    core_init(&core);

    // create the window
    struct window_t window;
    window_init(&window,
                CAKE_WINDOW_WIDTH,
                CAKE_WINDOW_HEIGHT,
                "cake",
                true);

    // create the cake instance and its output
    struct cake_t cake;

    struct instance_output_imgui_t instance_output;
    instance_output_imgui_init(&instance_output);

    struct instance_t instance;
    instance_init(&instance,
                  &window,
                  CAKE_RENDER_WIDTH,
                  CAKE_RENDER_HEIGHT,
                  &cake,
                  cake_init,
                  cake_deinit,
                  cake_render,
                  instance_output.backing);

    // run the instance and the main thread loop
    instance_run(&instance);
    while (instance_is_running(&instance))
        core_poll_events(&core, true);

    // deinit
    instance_deinit(&instance);
    instance_output_imgui_deinit(&instance_output);
    core_deinit(&core);
    return EXIT_SUCCESS;
}
