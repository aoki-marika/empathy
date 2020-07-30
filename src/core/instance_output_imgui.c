#include "instance_output_imgui.h"

#include <stdlib.h>
#include <string.h>

// MARK: - Functions

/// The initialization function for an IMGUI instance output's backing.
///
/// See `instance_init_function_t` for further documentation.
void instance_output_imgui_output_init(struct instance_t *instance,
                                       void *data)
{
    struct instance_output_imgui_t *output = (struct instance_output_imgui_t *)data;

    // init imgui
    imgui_init(&output->imgui, instance->window);
}

/// The deinitialization function for an IMGUI instance output's backing.
///
/// See `instance_deinit_function_t` for further documentation.
void instance_output_imgui_output_deinit(struct instance_t *instance,
                                         void *data)
{
    struct instance_output_imgui_t *output = (struct instance_output_imgui_t *)data;

    // deinit imgui
    imgui_deinit(&output->imgui);
}

/// The render function for an IMGUI instance output's backing.
///
/// See `instance_output_render_function_t` for further documentation.
void instance_output_imgui_output_render(struct instance_t *instance,
                                         void *data,
                                         struct framebuffer_t *framebuffer)
{
    struct instance_output_imgui_t *output = (struct instance_output_imgui_t *)data;

    // render the imgui frame
    imgui_begin_frame(&output->imgui);
        // main menu bar
        igBeginMainMenuBar();
            // tools menu
            // displays a list of added tools, where clicking on one toggles that tool
            if (igBeginMenu("Tools", true))
            {
                for (int i = 0; i < output->num_tools; i++)
                {
                    struct instance_output_imgui_tool_t *tool = &output->tools[i];
                    igMenuItemBoolPtr(tool->name, "", &tool->is_open, true);
                }

                igEndMenu();
            }
        igEndMainMenuBar();

        // tools
        // render each open tool
        for (int i = 0; i < output->num_tools; i++)
        {
            struct instance_output_imgui_tool_t *tool = &output->tools[i];
            if (tool->is_open)
                tool->render(output, tool, instance);
        }
    imgui_end_frame(&output->imgui);
}

/// The render function for the default program tool of an IMGUI instance output.
///
/// The program tool displays the final framebuffer of an instance's program within a window.
/// See `instance_output_imgui_tool_render_function_t` for further documentation.
void instance_output_imgui_tool_program_render(struct instance_output_imgui_t *output,
                                               struct instance_output_imgui_tool_t *tool,
                                               struct instance_t *instance)
{
    // get the final framebuffer and display it within a window
    const struct framebuffer_t *framebuffer = &instance->program.framebuffer;
    unsigned int width = framebuffer->texture.width;
    unsigned int height = framebuffer->texture.height;
    igBegin("Program", &tool->is_open, ImGuiWindowFlags_NoResize);
        igImage((void *)(intptr_t)framebuffer->texture.id,
                (ImVec2) { width, height },
                (ImVec2) { 0, 1 },
                (ImVec2) { 1, 0 },
                (ImVec4) { 1, 1, 1, 1 },
                (ImVec4) { 0, 0, 0, 0 });
    igEnd();
}

/// The render function for the default frame rate tool of an IMGUI instance output.
///
/// The frame rate tool displays the average frame time and duration of an instance's program within a window.
/// See `instance_output_imgui_tool_render_function_t` for further documentation.
void instance_output_imgui_tool_framerate_render(struct instance_output_imgui_t *output,
                                                 struct instance_output_imgui_tool_t *tool,
                                                 struct instance_t *instance)
{
    // get the frame rate and display it within a window
    // TODO: measure program frame time, as imgui only measures frame rate
    float framerate = output->imgui.io->Framerate;
    igBegin("Frame Rate", &tool->is_open, ImGuiWindowFlags_NoResize);
        igText("Average %.3f ms/frame (%.1f FPS)", 1000 / framerate, framerate);
    igEnd();
}

void instance_output_imgui_init(struct instance_output_imgui_t *output)
{
    // initialize the backing output
    struct instance_output_t *backing = &output->backing;
    backing->data = output;
    backing->init = instance_output_imgui_output_init;
    backing->deinit = instance_output_imgui_output_deinit;
    backing->render = instance_output_imgui_output_render;

    // initialize the tools
    output->num_tools = 0;
    output->tools = malloc(output->num_tools * sizeof(struct instance_output_imgui_tool_t));

    // add the default tools
    // program
    instance_output_imgui_add_tool(output,
                                   "Program",
                                   instance_output_imgui_tool_program_render,
                                   true);

    // frame rate
    instance_output_imgui_add_tool(output,
                                   "Frame Rate",
                                   instance_output_imgui_tool_framerate_render,
                                   true);
}

void instance_output_imgui_deinit(struct instance_output_imgui_t *output)
{
    for (int i = 0; i < output->num_tools; i++)
        free(output->tools[i].name);

    free(output->tools);
}

void instance_output_imgui_add_tool(struct instance_output_imgui_t *output,
                                    const char *name,
                                    instance_output_imgui_tool_render_function_t render,
                                    bool is_open)
{
    // insert the new tool
    unsigned int index = output->num_tools++;
    output->tools = realloc(output->tools, output->num_tools * sizeof(struct instance_output_imgui_tool_t));

    // get and initialize the new tool
    struct instance_output_imgui_tool_t *tool = &output->tools[index];
    tool->render = render;
    tool->is_open = is_open;

    // copy the name to avoid having to keep the given name accessible
    size_t name_size = strlen(name) + 1;
    char *name_copy = (char *)malloc(name_size * sizeof(char));
    strcpy(name_copy, name);
    tool->name = name_copy;
}
