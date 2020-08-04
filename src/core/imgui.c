#include "imgui.h"

#include <stdlib.h>

#include "platform.h"

// MARK: - Functions

void imgui_init(struct imgui_t *imgui,
                const struct window_t *window,
                const char *ini_name)
{
    // create the context
    struct ImGuiContext *context = igCreateContext(NULL);
    struct ImGuiIO *io = igGetIO();

    // get and set the ini path
    char *ini_path = platform_get_relative_path(ini_name);
    io->IniFilename = ini_path;

    // initialize imgui
    ImGui_ImplGlfw_InitForOpenGL(window->backing, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // intialize the given context
    imgui->ini_path = ini_path;
    imgui->context = context;
    imgui->io = io;
}

void imgui_deinit(struct imgui_t *imgui)
{
    // deinit imgui and the context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(imgui->context);
    free(imgui->ini_path);
}

void imgui_begin_frame(struct imgui_t *imgui)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();
}

void imgui_end_frame(struct imgui_t *imgui)
{
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
