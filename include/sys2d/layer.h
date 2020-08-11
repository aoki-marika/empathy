#pragma once

#include <core/vector.h>
#include <core/colour.h>
#include <core/texture.h>
#include <core/uv.h>
#include <core/mesh.h>
#include <core/matrix.h>

///
/// Layers are the core of Sys2D; defining scene graphs, their contents, rendering state, and providing drawing information.
///
/// A layer can have children, of which use their parent's properties as a base for their own when rendering.
/// For example, if a child has position `0,0` within a parent with position `50,50`, then the rendered position is `50,50`.
///
/// The visual contents of a layer are defined by "attachments".
/// Each attachment can be one of several types:
///  - Colour: The layer is a flat colour.
///  - Texture: The layer samples UV coordinates of a texture.
/// These attachments can then be switched between with animations to create effects such as a sprite animation.
///
/// For optimization, layers have their state rendered as little as possible.
/// To do this layers have "dirt"; an indication of properties that have changed since the last time the layer was rendered.
/// This then determines which parts of the layer need to be re-rendered when performing a render pass.
/// Render passes are performed automatically by various mutating layer functions, when appropriate.
///

// MARK: - Type Definitions

/// A unique identifier for a layer, within its parent's children.
typedef unsigned int layer_id_t;

// MARK: - Macros

/// The index of the vertex attribute that layer attachments bind their XYZ positions to.
///
/// XYZ coordinates are in pixels, with a top-left origin.
#define LAYER_ATTACHMENT_XYZ_ATTRIBUTE_INDEX           (0)

/// The index of the vertex attribute that layer attachments bind their RGBA colours to.
///
/// RGBA components are normalized from `0` to `1`.
#define LAYER_ATTACHMENT_RGBA_ATTRIBUTE_INDEX          (1)

/// The index of the vertex attribute that layer attachments bind their texture UV coordinates to.
///
/// UV components are normalized from `0` to `1`, with a bottom-left origin.
#define LAYER_ATTACHMENT_UV_ATTRIBUTE_INDEX            (2)

/// The index of the vertex attribute that layer attachments bind their texture array index to.
///
/// Texture indices are in elements.
#define LAYER_ATTACHMENT_TEXTURE_INDEX_ATTRIBUTE_INDEX (3)

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
        /// Attachment's rendered states.
        LAYER_ATTACHMENTS = 1 << 0,

        /// Transform model matrices.
        LAYER_TRANSFORM   = 1 << 1,
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

    /// The total number of attachments attached to this layer.
    unsigned int num_attachments;

    /// All the attachments attached to this layer.
    ///
    /// Allocated.
    struct layer_attachment_t
    {
        /// The type of this attachment.
        enum layer_attachment_type_t
        {
            /// The layer renders colours in each corner which it interpolates between.
            LAYER_ATTACHMENT_COLOUR,

            /// The layer renders a texture, sampling it using UV bounds.
            LAYER_ATTACHMENT_TEXTURE,
        } type;

        ///
        /// `LAYER_ATTACHMENT_COLOUR` properties.
        ///

        /// The colour of the top-left corner of this attachment.
        struct colour4_t colour_top_left;

        /// The colour of the top-right corner of this attachment.
        struct colour4_t colour_top_right;

        /// The colour of the bottom-left corner of this attachment.
        struct colour4_t colour_bottom_left;

        /// The colour of the bottom-right corner of this attachment.
        struct colour4_t colour_bottom_right;

        ///
        /// `LAYER_ATTACHMENT_TEXTURE` properties.
        ///

        /// The texture of which this attachment samples.
        ///
        /// The lifetime of this texture is handled by the creator of this attachment.
        struct texture_t *texture;

        /// The index, within this attachment's array texture, of the texture that this attachment samples.
        ///
        /// This is only used if `texture` is of type `TEXTURE_2D_ARRAY`.
        unsigned int texture_index;

        /// The bottom-left UV coordinates of the bounds that this attachment samples its texture from.
        struct uv_t texture_bottom_left;

        /// The top-right UV coordinates of the bounds that this attachment samples its texture from.
        struct uv_t texture_top_right;

        ///
        /// Render state properties.
        ///

        /// The last rendered state of this attachment.
        struct layer_attachment_rendered_state_t
        {
            /// The mesh used to draw the attachment, if any.
            ///
            /// Only size is accounted for in this mesh;
            /// anchor, origin, position, and other animatable or parent-relative properties must be applied when drawing.
            /// Allocated.
            struct mesh_t *mesh;
        } rendered_state;
    } *attachments;

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

/// Initialize the given layer as a root layer with the given parameters.
/// @param layer The layer to initialize.
/// @param size The size of the new layer, in pixels.
/// @param num_attachments The total number of given attachments.
/// @param attachments All the attachments to attach to the new layer.
/// The elements of this array are copied, so it does not need to remain accessible.
/// It is expected that the properties of each attachment are available for the entire lifetime of the new layer.
void layer_init(struct layer_t *layer,
                struct vector2_t size,
                unsigned int num_attachments,
                const struct layer_attachment_t *attachments);

/// Deinitialize the given layer and all its children, releasing all of their allocated resources.
/// @param layer The layer to deinitialize.
void layer_deinit(struct layer_t *layer);

/// Add a new child layer to the given layer's children with the given parameters.
/// @param layer The layer to add the new child layer to the children of.
/// It is expected that this layer is available for the entire lifetime of the new child layer.
/// @param child_id The pointer to set the value of to the unique identifier of the new child layer, within the given layer's children.
/// If this is `NULL` then it is not set.
/// @param anchor The normalized point, within the new child layer's parent, that it anchors its centre to.
/// @param origin The normalized point, within the new child layer, that it centres itself on.
/// @param size The size of the new child layer, in pixels.
/// @param num_attachments The total number of given attachments.
/// @param attachments All the attachments to attach to the new child layer.
/// The elements of this array are copied, so it does not need to remain accessible.
/// It is expected that the properties of each attachment are available for the entire lifetime of the new child layer.
void layer_add_child(struct layer_t *layer,
                     layer_id_t *child_id,
                     struct vector2_t anchor,
                     struct vector2_t origin,
                     struct vector2_t size,
                     unsigned int num_attachments,
                     const struct layer_attachment_t *attachments);

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

/// Add the given attachment to the given layer.
/// @param layer The layer to add the attachment to.
/// @param attachment The attachment to add.
/// It is expected that the properties of this attachment are available for the entire lifetime of the given layer.
void layer_add_attachment(struct layer_t *layer,
                          struct layer_attachment_t attachment);

/// Remove the attachment at the given index from the given layer.
///
/// If the given index is out of bounds of the given layer's attachments then an assertion fails.
/// @param layer The layer to remove the attachment from.
/// @param index The index of the attachment to remove within the given layer's attachments.
void layer_remove_attachment(struct layer_t *layer,
                             unsigned int index);
