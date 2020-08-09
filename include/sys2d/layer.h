#pragma once

#include <core/vector.h>
#include <core/colour.h>

///
/// Layers are the basis of sys2d, defining scene graphs and their contents.
///
/// A layer can have children, of which use their parent's properties as a base for their own when rendering.
/// For example, if a child has position `0,0` within a parent with position `50,50`, then the final render position is `50,50`.
///
/// The visual contents of a layer are defined by "attachments".
/// Each attachment can be one of several types:
///  - Text: The layer renders text using a font.
///  - Texture: The layer samples UV coordinates of a texture.
///  - Colour: The layer is a flat colour.
/// These attachments can then be switched between with animations to create effects such as a sprite animation.
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
            /// The layer renders a solid colour.
            ///
            /// Uses `colour`.
            LAYER_ATTACHMENT_COLOUR,
        } type;

        ///
        /// Different properties are used depending on the attachment type.
        /// See `layer_attach_type_t` case documentation to determine which are used by each type.
        ///

        /// The colour property of this attachment.
        struct colour4_t colour;
    } *attachments;

    /// The unique identifier for the next child layer added to this layer.
    layer_id_t next_child_id;

    /// The total number of child layers within this layer.
    unsigned int num_children;

    /// All the child layers within this layer.
    ///
    /// Allocated.
    struct layer_t *children;
};

// MARK: - Functions

/// Initialize the given layer as a root layer with the given properties and attachments.
/// @param layer The layer to initialize.
/// @param properties The properties of the new layer.
/// @param num_attachments The total number of given attachments.
/// @param attachments All the attachments to attach to the new layer.
/// The elements of this array are copied, so it does not need to remain accessible.
/// It is expected that the properties of each attachment are available for the entire lifetime of the new layer.
void layer_init(struct layer_t *layer,
                struct layer_properties_t properties,
                unsigned int num_attachments,
                const struct layer_attachment_t *attachments);

/// Deinitialize the given layer and all its children, releasing all of their allocated resources.
/// @param layer The layer to deinitialize.
void layer_deinit(struct layer_t *layer);

/// Add a new child layer to the given parent layer's children with the given properties and attachments.
/// @param child_id The pointer to set the value of to the unique identifier of the new child layer, within the given parent's children.
/// If this is `NULL` then it is not set.
/// @param parent The layer to add the new child layer to the children of.
/// It is expected that this layer is available for the entire lifetime of the new child layer.
/// @param properties The properties of the new child layer.
/// @param num_attachments The total number of given attachments.
/// @param attachments All the attachments to attach to the new child layer.
/// The elements of this array are copied, so it does not need to remain accessible.
/// It is expected that the properties of each attachment are available for the entire lifetime of the new child layer.
void layer_add(layer_id_t *child_id,
               struct layer_t *parent,
               struct layer_properties_t properties,
               unsigned int num_attachments,
               const struct layer_attachment_t *attachments);

/// Remove the first child layer matching the given unique identifier within the given parent's children, deinitializing it.
///
/// If no child matching the given unique identifier is found within the given parent's children then the program terminates.
/// @param child_id The unique identifier of the child layer to remove.
/// @param parent The layer containing the layer to remove.
void layer_remove(layer_id_t child_id,
                  struct layer_t *parent);

/// Get the first child layer matching the given unique identifier within the given parent's children, if any.
/// @param child_id The unique identifier of the child layer to get.
/// @param parent The layer containing the layer to get.
/// @return A pointer to the first child layer matching the given unique identifier within the given parent's children.
/// This pointer is only accessible until the given parent's children are modified.
/// If no matches were found then `NULL` is returned instead.
struct layer_t *layer_get(layer_id_t child_id,
                          struct layer_t *parent);

/// Set the properties of the given layer to the given properties.
/// @param layer The layer to set the properties of.
/// @param properties The properties to set.
void layer_set_properties(struct layer_t *layer,
                          struct layer_properties_t properties);
