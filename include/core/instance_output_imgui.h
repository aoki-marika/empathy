#pragma once

#include "imgui.h"
#include "instance.h"

///
/// An IMGUI instance output displays the instance within an IMGUI interface, with various configurable tools.
///
/// "Tools" within an IMGUI instance output refers to a single, isolated IMGUI interface for displaying a debugging tool for the instance's program.
/// These can range anywhere from displays of runtime state to modification of subsystems.
/// By default IMGUI instance outputs include the following tools:
///  - Program: Displays the final framebuffer of the instance's program.
///  - Frame Rate: Displays average frame time and rate.
/// Tools can be opened and closed via the menu bar and, if the tool supports it, the "X" button on the window.
///
/// Generally when using an IMGUI instance output the window should be larger than the instance's render size and resizable,
/// making more space for the tools while easily viewing the entire final framebuffer.
///

// MARK: - Type Definitions

struct instance_output_imgui_t;
struct instance_output_imgui_tool_t;

/// A function which is used to render a single IMGUI frame of a single tool within an IMGUI instance output.
///
/// The IMGUI context of the given output has already begun its frame when this function is called.
/// @param output The output which is rendering the given tool.
/// @param tool The tool which is being rendered.
/// This is useful for tools that create a window to bind to `is_open` to allow closing it with the `X` button.
/// @param instance The instance which is using the given output.
typedef void (* instance_output_imgui_tool_render_function_t)(struct instance_output_imgui_t *output,
                                                              struct instance_output_imgui_tool_t *tool,
                                                              struct instance_t *instance);

// MARK: - Data Structures

/// An IMGUI instance output.
struct instance_output_imgui_t
{
    /// The backing output of this output.
    ///
    /// This is the actual output which is passed to an instance.
    struct instance_output_t backing;

    /// The total number of tools within this output.
    unsigned int num_tools;

    /// All the tools within this output.
    ///
    /// Allocated.
    struct instance_output_imgui_tool_t
    {
        /// The unique display name of this tool.
        ///
        /// Allocated.
        char *name;

        /// The function used to render a single IMGUI frame of this tool.
        instance_output_imgui_tool_render_function_t render;

        /// Whether or not this tool is currently open.
        bool is_open;
    } *tools;

    /// The IMGUI context of this output.
    ///
    /// Managed by the backing output's functions.
    struct imgui_t imgui;
};

// MARK: - Functions

/// Initialize the given IMGUI instance output.
///
/// The new IMGUI output automatically has the default program and frame rate tools added.
/// Due to the limitations of the IMGUI implementations,
/// only a single IMGUI instance output can be created per core program,
/// and cannot be shared across instances.
/// @param output The output to initialize.
void instance_output_imgui_init(struct instance_output_imgui_t *output);

/// Deinitialize the given IMGUI instance output, releasing all of its allocated resources.
/// @param output The output to deinitialize.
void instance_output_imgui_deinit(struct instance_output_imgui_t *output);

/// Add a new tool from the given properties to the given IMGUI instance output.
///
/// It is generally recommended to add all the tools before using an output.
/// @param output The output to add the new tool to.
/// @param name The unique display name of the new tool.
/// This string is copied so it does not need to remain accessible.
/// @param render The function to call to render a single IMGUI frame of the new tool.
/// See `instance_output_imgui_tool_render_function_t` for further documentation.
/// @param is_open Whether or not the new tool is open by default.
void instance_output_imgui_add_tool(struct instance_output_imgui_t *output,
                                    const char *name,
                                    instance_output_imgui_tool_render_function_t render,
                                    bool is_open);
