#pragma once

#include <core/vector.h>
#include <core/matrix.h>

#include "attachment.h"

///
/// Layers are the core of Sys2D, defining scene graphs, their nodes, rendering state, and providing layout information.
///
/// A layer can have children, of which use their parent's properties as a base for their own when rendering.
/// For example, if a child has position `0,0` within a parent with position `50,50`, then the rendered position is `50,50`.
///
/// For optimization, layers have their state rendered as little as possible.
/// To do this layers have "dirt"; an indication of properties that have changed since the last time the layer was rendered.
/// This then determines which parts of the layer need to be re-rendered when performing a render pass.
///

// MARK: - Type Definitions

/// A unique identifier for a layer, within its parent's children.
typedef unsigned int layer_id_t;

// MARK: - Data Structures

/// A single layer.
struct layer_t
{
    /// The unique identifier of this layer within its parent's children.
    layer_id_t id;

    /// The properties of this layer.
    struct layer_properties_t
    {
        /// The normalized point, within this layer's parent, that this layer anchors its centre to.
        struct vector2_t anchor;

        /// The normalized point, within this layer, that this layer centres itself on.
        struct vector2_t origin;

        /// The size of this layer, in pixels.
        struct vector2_t size;
    } properties;

    /// All the properties of this layer affected by changes which have occurred within this layer since the last render pass.
    enum layer_dirt_t
    {
        /// None.
        LAYER_NONE      = 0 << 0,

        /// Transform matrix.
        LAYER_TRANSFORM = 1 << 0,
    } dirt;

    /// The last rendered state of this layer.
    struct layer_rendered_state_t
    {
        /// The size of the layer's parent, in pixels.
        struct vector2_t parent_size;

        /// The world-space transform model matrix of the layer's parent.
        struct matrix4_t parent_transform_world;

        /// The world-space transform model matrix of the layer.
        struct matrix4_t transform_world;
    } rendered_state;

    /// The unique identifier for the next attachment added to this layer.
    attachment_id_t next_attachment_id;

    /// The total number of attachments attached to this layer.
    unsigned int num_attachments;

    /// All the attachments attached to this layer.
    ///
    /// Allocated.
    struct attachment_t *attachments;

    /// The unique identifier for the next child layer added to this layer.
    layer_id_t next_child_id;

    /// The total number of child layers within this layer.
    unsigned int num_children;

    /// All the child layers within this layer.
    ///
    /// Children are ordererd back-to-front, on top of the parent.
    /// Allocated.
    struct layer_t *children;
};

// MARK: - Functions

// MARK: Layer

/// Initialize the given layer as a root layer with the given properties.
/// @param layer The layer to initialize.
/// See `layer_properties_t` for property documentation.
void layer_init(struct layer_t *layer,
                struct vector2_t size);

/// Deinitialize the given layer and all its children, releasing all of their allocated resources.
/// @param layer The layer to deinitialize.
void layer_deinit(struct layer_t *layer);

///
/// Layer property set functions.
/// See `layer_properties_t` for documentation on the properties set by each.
///

void layer_set_anchor(struct layer_t *layer,
                      struct vector2_t value);

void layer_set_origin(struct layer_t *layer,
                      struct vector2_t value);

void layer_set_size(struct layer_t *layer,
                    struct vector2_t value);

/// Perform a render pass on the given layer and its children, re-rendering any properties that have changed since the last render pass.
///
/// This must be called before the given layer is drawn when it is initialized or when its properties change.
/// Layers on their own will not perform a render pass, which will leave the rendered state empty.
/// @param layer The layer to render.
void layer_render(struct layer_t *layer);

// MARK: Children

/// Add a new child layer to the given layer's children with the given properties.
/// @param layer The layer to add the new child layer to the children of.
/// @param child_id The pointer to set the value of to the unique identifier of the new child layer, within the given layer's children.
/// If this is `NULL` then it is not set.
/// See `layer_properties_t` for property documentation.
void layer_add_child(struct layer_t *layer,
                     layer_id_t *child_id,
                     struct vector2_t anchor,
                     struct vector2_t origin,
                     struct vector2_t size);

/// Remove the first child layer matching the given unique identifier within the given layer's children, deinitializing it.
///
/// If no child matching the given unique identifier is found within the given layer's children then the program terminates.
/// @param layer The layer containing the layer to remove.
/// @param child_id The unique identifier of the child layer to remove.
void layer_remove_child(struct layer_t *layer,
                        layer_id_t child_id);

/// Get the first child layer matching the given unique identifier within the given layer's children, if any.
/// @param layer The layer containing the layer to get.
/// @param child_id The unique identifier of the child layer to get.
/// @return A pointer to the first child layer matching the given unique identifier within the given layer's children.
/// This pointer is only available until the given layer's children are modified.
/// If no matches were found then `NULL` is returned instead.
struct layer_t *layer_get_child(struct layer_t *layer,
                                layer_id_t child_id);

// MARK: Attachments

/// Add a new colour attachment with the given properties to the given layer's attachments.
/// @param layer The layer to add the new colour attachment to.
/// @param id The pointer to set the value of to the unique identifier of the new colour attachment, within the given layer's attachments.
/// If this is `NULL` then it is not set.
/// See `attachment_colour_t` for property documentation.
void layer_add_attachment_colour(struct layer_t *layer,
                                 struct colour4_t top_left,
                                 struct colour4_t top_right,
                                 struct colour4_t bottom_left,
                                 struct colour4_t bottom_right);

/// Add a new texture attachment with the given properties to the given layer's attachments.
/// @param layer The layer to add the new texture attachment to.
/// @param id The pointer to set the value of to the unique identifier of the new texture attachment, within the given layer's attachments.
/// If this is `NULL` then it is not set.
/// See `attachment_texture_t` for property documentation.
void layer_add_attachment_texture(struct layer_t *layer,
                                  struct texture_t *source,
                                  unsigned int source_index,
                                  struct uv_t bottom_left,
                                  struct uv_t top_right);

/// Remove the first attachment matching the given unique identifier within the given layer's attachments, deinitializing it.
///
/// If no attachment matching the given unique identifier is found within the given layer's attachment then the program terminates.
/// @param layer The layer containing the attachment to remove.
/// @param id The unique identifier of the attachment to remove.
void layer_remove_attachment(struct layer_t *layer,
                             attachment_id_t id);

/// Get the first attachment matching the given unique identifier within the given layer's attachments, if any.
/// @param layer The layer containing the attachment to get.
/// @param id The unique identifier of the attachment to get.
/// @return A pointer to the first attachment matching the given unique identifier within the given layer's attachments.
/// This pointer is only available until the given layer's attachments are modified.
/// If no matches were found then `NULL` is returned instead.
struct attachment_t *layer_get_attachment(struct layer_t *layer,
                                          attachment_id_t id);
