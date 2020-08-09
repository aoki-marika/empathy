#pragma once

#include <core/vector.h>

///
/// Layers are the basis of sys2d, defining scene graphs and their contents.
///
/// A layer can have children, of which use their parent's properties as a base for their own when rendering.
/// For example, if a child has position `0,0` within a parent with position `50,50`, then the final render position is `50,50`.
///
/// The visual contents of a layer are defined by "attachments".
/// Each attachment can be one of several types:
///  - Texture: The layer samples UV coordinates of a texture.
///  - Text: The layer renders text using a font.
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

/// Initialize the given layer as a root layer with the given properties.
/// @param layer The layer to initialize.
/// @param properties The properties of the new layer.
void layer_init_root(struct layer_t *layer,
                     struct layer_properties_t properties);

/// Insert a new child layer into the given layer with the given properties.
/// @param child_id The pointer to set the value of to the unique identifier of the new child layer, within the given parent's children.
/// If this is `NULL` then it is not set.
/// @param properties The properties of the new child layer.
void layer_init_child(layer_id_t *child_id,
                      struct layer_t *parent,
                      struct layer_properties_t properties);

/// Deinitialize the given layer and all its children, releasing all of their allocated resources.
/// @param layer The layer to deinitialize.
void layer_deinit(struct layer_t *layer);

/// Get the first child layer matching the given unique identifier within the given parent's children, if any.
/// @param child_id The unique identifier of the child layer to get.
/// @param parent The layer containing the layer to get.
/// @return A pointer to the first child layer matching the given unique identifier within the given parent's children.
/// This pointer is only accessible until the given parent's children are modified.
/// If no matches were found then `NULL` is returned instead.
struct layer_t *layer_get_child(layer_id_t child_id,
                                struct layer_t *parent);

/// Set the properties of the given layer to the given properties.
/// @param layer The layer to set the properties of.
/// @param properties The properties to set.
void layer_set_properties(struct layer_t *layer,
                          struct layer_properties_t properties);

/// Remove the first child layer matching the given unique identifier within the given parent's children, deinitializing it.
///
/// If no child matching the given unique identifier is found within the given parent's children then the program terminates.
/// @param child_id The unique identifier of the child layer to remove.
/// @param parent The layer containing the layer to remove.
void layer_remove(layer_id_t child_id,
                  struct layer_t *parent);
