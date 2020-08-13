#pragma once

#include <core/program.h>

#include "layer.h"

///
/// Drawers are used to hold common state which is used to draw the render results of layers to graphics contexts.
///
/// Drawers are not tied to individual layers, instead there is intended to be one drawer per-program which draws all the layers within said program.
///

// MARK: - Macros

/// The texture unit that drawers bind textures to when drawing texture attachments.
#define DRAWER_UNIT 1

// MARK: - Data Structures

/// A layer drawer.
struct drawer_t
{
    /// The shared attachment vertex shader of this drawer.
    struct shader_t vertex;

    /// The colour attachment fragment shader of this drawer.
    struct shader_t fragment_colour;

    /// The 2D texture attachment fragment shader of this drawer.
    struct shader_t fragment_texture_2d;

    /// The 2D array texture attachment fragment shader of this drawer.
    struct shader_t fragment_texture_2d_array;

    /// The colour attachment shader program of this drawer.
    struct program_t program_colour;

    /// The 2D texture attachment shader program of this drawer.
    struct program_t program_texture_2d;

    /// The 2D array texture attachment shader program of this drawer.
    struct program_t program_texture_2d_array;
};

// MARK: - Functions

/// Initialize the given drawer with the given draw size.
/// @param drawer The drawer to initialize.
/// @param draw_width The width that the new drawer draws at, in pixels.
/// @param draw_height The height that the new drawer draws at, in pixels.
void drawer_init(struct drawer_t *drawer,
                 unsigned int draw_width,
                 unsigned int draw_height);

/// Deinitialize the given drawer, releasing all of its allocated resources.
/// @param drawer The drawer to deinitialize.
void drawer_deinit(struct drawer_t *drawer);

/// Draw the last render result of the given layer and its children using the given drawer to the current graphics context.
/// @param layer The layer to draw.
/// @param drawer The drawer to draw the given layer with.
void layer_draw(const struct layer_t *layer,
                struct drawer_t *drawer);
