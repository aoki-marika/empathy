#pragma once

#include <stdbool.h>
#include <core/vector.h>
#include <core/matrix.h>

#include "attachment.h"

///
/// Layers are the core of Sys2D, defining scene graphs, their nodes, rendering state, and providing layout information.
///
/// A layer can have children, of which use their parent's properties as a base for their own when rendering.
/// For example, if a child has position `0,0` within a parent with position `50,50`, then the rendered position is `50,50`.
///
/// Layers are uniquely identified in two ways:
///  - Identifiers: The unique identifier of a layer within its parent's children.
///  - Paths: The absolute path of a layer within its root layer's hierarchy.
///           These can be thought of like filesystem paths; a number of components that form a way to identify an item within a hierarchy.
/// As identifiers are only unique within a single layer's children, they have limited use cases.
/// Paths on the other hand are unique within a root layer's hierarchy, so they are generally preferred in cases where the root layer is available.
///
/// For optimization, layers have their state rendered as little as possible.
/// To do this layers have "dirt"; an indication of properties that have changed since the last time the layer was rendered.
/// This then determines which parts of the layer need to be re-rendered when performing a render pass.
///

// MARK: - Type Definitions

/// A unique identifier for a layer, within its parent's children.
typedef unsigned int layer_id_t;

// MARK: - Macros

/// The maximum number of layer identifiers that can be used to form a single path.
#define LAYER_PATH_MAX_IDS UINT8_MAX

// MARK: - Data Structures

/// A single layer.
struct layer_t
{
    /// The unique identifier of this layer within its parent's children.
    layer_id_t id;

    /// The unique path of this layer within its root layer's hierarchy.
    struct layer_path_t
    {
        /// The total number of layer identifiers that form this path.
        int num_ids;

        /// All the layer identifiers that form this path.
        ///
        /// Paths are used by following these identifiers down the hierarchy of the root layer.
        layer_id_t ids[LAYER_PATH_MAX_IDS];
    } path;

    /// The properties of this layer.
    struct layer_properties_t
    {
        /// The normalized point, within this layer's parent, that this layer anchors its centre to.
        struct vector2_t anchor;

        /// The normalized point, within this layer, that this layer centres itself on.
        struct vector2_t origin;

        /// The position of this layer's origin, relative to its anchor, in pixels.
        struct vector2_t position;

        /// The size of this layer, in pixels.
        struct vector2_t size;

        /// The normalized scale of this layer, around its origin.
        struct vector2_t scale;

        /// The size-relative normalized shear of this layer.
        struct vector2_t shear;

        /// The clockwise rotation of this layer, around its origin, in degrees.
        float rotation;
    } properties;

    /// All the properties of this layer affected by changes which have occurred within this layer since the last render pass.
    enum layer_dirt_t
    {
        /// None.
        LAYER_NONE      = 0 << 0,

        /// Transform matrix.
        LAYER_TRANSFORM = 1 << 0,
    } dirt;

    /// The result of the last render pass performed on this layer.
    struct layer_render_result_t
    {
        /// The size of the layer's parent, in pixels.
        struct vector2_t parent_size;

        /// The world-space transform model matrix of the layer's parent.
        struct matrix4_t parent_transform_world;

        /// The world-space transform model matrix of the layer.
        struct matrix4_t transform_world;
    } render_result;

    /// The total number of attachments attached to this layer.
    unsigned int num_attachments;

    /// All the attachments attached to this layer.
    ///
    /// The array and each item are allocated.
    struct attachment_t **attachments;

    /// The unique identifier for the next child layer added to this layer.
    layer_id_t next_child_id;

    /// The total number of child layers within this layer.
    unsigned int num_children;

    /// All the child layers within this layer.
    ///
    /// Children are ordererd back-to-front, on top of the parent.
    /// The array and each item are allocated.
    struct layer_t **children;
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

/// Get whether or not the given layer's path matches the given path.
/// @param layer The layer to compare the path of.
/// @param path The path to compare against.
/// @return Whether or not the given layer's path matches the given path.
bool layer_path_equals(const struct layer_t *layer,
                       const struct layer_path_t *path);

/// Perform a render pass on the given layer and its children, re-rendering any properties that have changed since the last render pass.
///
/// This must be called before the given layer is drawn when it is initialized or when its properties change.
/// Layers on their own will not perform a render pass, which will leave the render result empty.
/// @param layer The layer to render.
void layer_render(struct layer_t *layer);

///
/// Layer property set functions.
/// See `layer_properties_t` for documentation on the properties set by each.
///

void layer_set_anchor(struct layer_t *layer,
                      struct vector2_t value);

void layer_set_origin(struct layer_t *layer,
                      struct vector2_t value);

void layer_set_position(struct layer_t *layer,
                        struct vector2_t value);

void layer_set_size(struct layer_t *layer,
                    struct vector2_t value);

void layer_set_scale(struct layer_t *layer,
                     struct vector2_t value);

void layer_set_shear(struct layer_t *layer,
                     struct vector2_t value);

void layer_set_rotation(struct layer_t *layer,
                        float value);

// MARK: Children

/// Add a new child layer to the given layer's children with the given properties.
/// @param layer The layer to add the new child layer to the children of.
/// @param child_id The pointer to set the value of to the unique identifier of the new child layer, within the given layer's children.
/// If this is `NULL` then it is not set.
/// @param child_path The pointer to set the value of to the unique path of the new child layer, within the given layer's root layer's hierarchy.
/// If this is `NULL` then it is not set.
/// See `layer_properties_t` for property documentation.
void layer_add_child(struct layer_t *layer,
                     layer_id_t *child_id,
                     struct layer_path_t *child_path,
                     struct vector2_t anchor,
                     struct vector2_t origin,
                     struct vector2_t position,
                     struct vector2_t size,
                     struct vector2_t shear,
                     struct vector2_t scale,
                     float rotation);

/// Remove the first child layer matching the given unique identifier within the given layer's children, deinitializing it.
///
/// If no child matching the given unique identifier is found within the given layer's children then the program terminates.
/// @param layer The layer containing the layer to remove.
/// @param child_id The unique identifier of the child layer to remove.
void layer_remove_child(struct layer_t *layer,
                        layer_id_t child_id);

/// Get the first child layer matching the given unique identifier within the given layer's children, if any.
/// @param layer The layer containing the child layer to get.
/// @param child_id The unique identifier of the child layer to get.
/// @return A pointer to the first child layer matching the given unique identifier within the given layer's children.
/// This pointer is only available until the given layer's children are modified.
/// If no matches were found then `NULL` is returned instead.
struct layer_t *layer_get_child_id(struct layer_t *layer,
                                   layer_id_t child_id);

/// Get the child layer at the given unique path within the given root layer's hierarchy, if any.
/// @param root_layer The root layer that the given path is within.
/// @param child_path The unique path of the child layer to get.
/// @return A pointer to the child layer at the given unique path within the given root layer's hierarchy.
/// This pointer is only available until the given root layer's hierarchy is modified.
/// If there is no layer at the given path then `NULL` is returned instead.
struct layer_t *layer_get_child_path(struct layer_t *root_layer,
                                     const struct layer_path_t *child_path);

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

/// Remove the given attachment from the given layer's attachments.
///
/// If the given attachment is not within the given layer's attachments then the program terminates.
/// After this function the given attachment is no longer available.
/// @param layer The layer containing the given attachment.
/// @param attachment The attachment to remove.
void layer_remove_attachment(struct layer_t *layer,
                             const struct attachment_t *attachment);
