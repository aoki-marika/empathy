#include "imgui.h"

// MARK: - Functions

void imgui_init(struct imgui_t *imgui, const struct window_t *window)
{
    // create the context
    struct ImGuiContext *context = igCreateContext(NULL);
    struct ImGuiIO *io = igGetIO();

    // TODO: create the ini alongside the binary
    // the default implementation uses cwd which is annoying when developing
    // so for now it is disabled
    io->IniFilename = NULL;

    // initialize imgui
    ImGui_ImplGlfw_InitForOpenGL(window->backing, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // intialize the given context
    imgui->context = context;
    imgui->io = io;
}

void imgui_deinit(struct imgui_t *imgui)
{
    // deinit imgui and the context
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    igDestroyContext(imgui->context);
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
