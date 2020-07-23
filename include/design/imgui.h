#pragma once

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>

#include <core/window.h>

///
/// Utility wrappers to make IMGUI usage more friendly.
///

// MARK: - Data Structures

/// A single IMGUI context.
struct imgui_t
{
    /// The backing context of this IMGUI context.
    struct ImGuiContext *context;

    /// The IO of this IMGUI context.
    struct ImGuiIO *io;
};

// MARK: - Functions

/// Initialize the given IMGUI context within the given window.
/// @param imgui The IMGUI context to initialize.
/// @param window The window to create the new context within.
void imgui_init(struct imgui_t *imgui, const struct window_t *window);

/// Deinitialize the given IMGUI context, releasing all of it's allocated resources.
/// @param imgui The IMGUI context to deinitialize.
void imgui_deinit(struct imgui_t *imgui);

/// Begin a new frame within the given IMGUI context.
///
/// This must be called before any other IMGUI functions can be called during a single IMGUI frame.
/// @param imgui The IMGUI context to begin the new frame within.
void imgui_begin_frame(struct imgui_t *imgui);

/// End and display the current frame within the given IMGUI context.
///
/// This must be called at the end of every IMGUI frame to display it.
/// @param imgui The IMGUI context to end the current frame of.
void imgui_end_frame(struct imgui_t *imgui);
