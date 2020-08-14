#include "layer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// MARK: - Functions

// MARK: Layer

/// Add the given dirt to the given layer's, and optionally its children's, dirt.
/// @param layer The layer to add the given dirt to.
/// @param dirt The dirt to add.
/// @param include_children Whether or not the given layer's children should also have the given dirt added.
void layer_add_dirt(struct layer_t *layer,
                    enum layer_dirt_t dirt,
                    bool include_children)
{
    layer->dirt |= dirt;
    if (include_children)
        for (int i = 0; i < layer->num_children; i++)
            layer_add_dirt(&layer->children[i], dirt, true);
}

/// Initialize the given layer as a root layer with the given properties.
/// @param layer The layer to initialize.
/// @param parent_layer The parent layer of the new layer.
/// This can be `NULL` for root layers.
/// @param id The unique identifier of the new layer, within its parent's children.
/// See `layer_properties_t` for property documentation.
void layer_init_dirty(struct layer_t *layer,
                      const struct layer_t *parent_layer,
                      layer_id_t id,
                      struct vector2_t anchor,
                      struct vector2_t origin,
                      struct vector2_t size)
{
    // initialize the given layer
    // layer
    layer->id = id;
    layer->dirt = LAYER_TRANSFORM;

    // path
    // set defaults
    layer->path.num_ids = 0;
    memset(layer->path.ids, 0x0, sizeof(layer->path.ids));

    // populate the path if a parent was given
    // if no parent was given then this is a root layer and it has no path
    if (parent_layer != NULL)
    {
        // copy the given parents path to use as a base
        memcpy(layer->path.ids,
               parent_layer->path.ids,
               parent_layer->path.num_ids * sizeof(layer_id_t));

        // append the id of the new layer to the end of the path
        layer->path.num_ids = parent_layer->path.num_ids + 1;
        layer->path.ids[layer->path.num_ids - 1] = layer->id;
    }

    // properties
    layer->properties.anchor = anchor;
    layer->properties.origin = origin;
    layer->properties.size = size;

    // render result
    layer->render_result.parent_size = vector2_zero();
    layer->render_result.parent_transform_world = matrix4_identity();
    layer->render_result.transform_world = matrix4_identity();

    // attachments
    layer->next_attachment_id = 0;
    layer->num_attachments = 0;
    layer->attachments = malloc(0);

    // children
    layer->next_child_id = 0;
    layer->num_children = 0;
    layer->children = malloc(0);
}

void layer_init(struct layer_t *layer,
                struct vector2_t size)
{
    // initialize the given layer
    layer_init_dirty(layer,
                     NULL,
                     0,
                     vector2_zero(),
                     vector2_zero(),
                     size);
}

void layer_deinit(struct layer_t *layer)
{
    // children
    for (int i = 0; i < layer->num_children; i++)
        layer_deinit(&layer->children[i]);
    free(layer->children);

    // attachments
    for (int i = 0; i < layer->num_attachments; i++)
        attachment_deinit(&layer->attachments[i]);
    free(layer->attachments);
}

bool layer_path_equals(const struct layer_t *layer,
                       const struct layer_path_t *rhs)
{
    // get the path to compare to the given path
    const struct layer_path_t *lhs = &layer->path;

    // if the id counts mismatch then the two paths are different
    // this is an easy check to avoid any uneccessary iterating of the ids
    if (lhs->num_ids != rhs->num_ids)
        return false;

    // compare each component of the paths
    // if any component mismatches then the paths are different
    for (int i = 0; i < lhs->num_ids; i++)
        if (lhs->ids[i] != rhs->ids[i])
            return false;

    // if this point has been reached then the two paths match
    return true;
}

void layer_render(struct layer_t *layer)
{
    // render the given layer, depending on its dirt
    enum layer_dirt_t dirt = layer->dirt;
    if (dirt & LAYER_TRANSFORM)
    {
        // the transform matrix needs to be recalculated
        // calculate and set the new matrix
        struct vector3_t absolute_anchor = vector3(layer->render_result.parent_size.x * layer->properties.anchor.x,
                                                   layer->render_result.parent_size.y * layer->properties.anchor.y,
                                                   0);

        struct vector3_t absolute_origin = vector3(-layer->properties.size.x * layer->properties.origin.x,
                                                   -layer->properties.size.y * layer->properties.origin.y,
                                                   0);

        struct matrix4_t model = layer->render_result.parent_transform_world;
        model = matrix4_multiply(model, matrix4_translation(absolute_origin));
        model = matrix4_multiply(model, matrix4_translation(absolute_anchor));
        layer->render_result.transform_world = model;
    }

    // perform a render pass on the given layers attachments
    for (int i = 0; i < layer->num_attachments; i++)
    {
        struct attachment_t *attachment = &layer->attachments[i];
        attachment_render(attachment, layer->properties.size);
    }

    // perform a render pass on the given layers children, passing any new render results from their parent
    for (int i = 0; i < layer->num_children; i++)
    {
        struct layer_t *child = &layer->children[i];

        if (dirt & LAYER_TRANSFORM)
        {
            child->render_result.parent_size = layer->properties.size;
            child->render_result.parent_transform_world = layer->render_result.transform_world;
        }

        layer_render(child);
    }

    // reset the given layers dirt, as it has now been applied
    layer->dirt = LAYER_NONE;
}

void layer_set_anchor(struct layer_t *layer,
                      struct vector2_t value)
{
    layer->properties.anchor = value;
    layer_add_dirt(layer, LAYER_TRANSFORM, true);
}

void layer_set_origin(struct layer_t *layer,
                      struct vector2_t value)
{
    layer->properties.origin = value;
    layer_add_dirt(layer, LAYER_TRANSFORM, true);
}

void layer_set_size(struct layer_t *layer,
                    struct vector2_t value)
{
    layer->properties.size = value;
    layer_add_dirt(layer, LAYER_TRANSFORM, true);

    // attachments need to re-render their mesh for size changes
    for (int i = 0; i < layer->num_attachments; i++)
        layer->attachments[i].dirt |= ATTACHMENT_MESH;
}

// MARK: Children

/// Get the index of the first child layer matching the given unique identifier within the given layer's children.
/// @param layer The layer containing the child layer to get the index of.
/// @param child_id The unique identifier of the child layer to get the index of.
/// @return The index of the first child layer matching the given unique identifier within the given layer's children.
/// If no match was found then `-1` is returned instead.
int layer_get_child_index(const struct layer_t *layer,
                          layer_id_t child_id)
{
    for (int i = 0; i < layer->num_children; i++)
    {
        const struct layer_t *child_layer = &layer->children[i];
        if (child_layer->id == child_id)
            return i;
    }

    // if this point has been reached then no match was found
    return -1;
}

void layer_add_child(struct layer_t *layer,
                     layer_id_t *child_id,
                     struct layer_path_t *child_path,
                     struct vector2_t anchor,
                     struct vector2_t origin,
                     struct vector2_t size)
{
    // insert the new child layer
    unsigned int child_index = layer->num_children++;
    layer->children = realloc(layer->children,
                              layer->num_children * sizeof(struct layer_t));

    struct layer_t *child_layer = &layer->children[child_index];

    // initialize the new child layer
    layer_init_dirty(child_layer,
                     layer,
                     layer->next_child_id++,
                     anchor,
                     origin,
                     size);

    // set the given pointer values
    if (child_id != NULL)
        *child_id = child_layer->id;

    if (child_path != NULL)
        *child_path = child_layer->path;
}

void layer_remove_child(struct layer_t *layer,
                        layer_id_t child_id)
{
    // attempt to get the index of the child layer to remove
    int child_index = layer_get_child_index(layer, child_id);
    if (child_index < 0)
    {
        // the child layer could not be found, print the details and terminate
        fprintf(stderr, "LAYER ERROR: could not locate child layer %u within layer %p\n", child_id, layer);
        exit(EXIT_FAILURE);
    }

    // deinitialize the child layer before removing it
    struct layer_t *child_layer = &layer->children[child_index];
    layer_deinit(child_layer);

    // shuffle and reallocate the layers children array to remove the child layers element
    memmove(&layer->children[child_index],
            &layer->children[child_index + 1],
            (layer->num_children - (child_index + 1)) * sizeof(struct layer_t));

    layer->num_children--;
    layer->children = realloc(layer->children,
                              layer->num_children * sizeof(struct layer_t));
}

struct layer_t *layer_get_child_id(struct layer_t *layer,
                                   layer_id_t child_id)
{
    int child_index = layer_get_child_index(layer, child_id);
    if (child_index >= 0)
        return &layer->children[child_index];
    else
        return NULL;
}

struct layer_t *layer_get_child_path(struct layer_t *root_layer,
                                     const struct layer_path_t *path)
{
    // follow the given path, returning early if any component is not found
    struct layer_t *layer = root_layer;
    for (int i = 0; i < path->num_ids; i++)
    {
        layer_id_t child_id = path->ids[i];
        struct layer_t *child_layer = layer_get_child_id(layer, child_id);
        if (child_layer == NULL)
            return NULL;

        layer = child_layer;
    }

    return layer;
}

// MARK: Attachment

/// Get the index of the first attachment matching the given unique identifier within the given layer's attachments.
/// @param layer The layer containing the attachment to get the index of.
/// @param id The unique identifier of the attachment to get the index of.
/// @return The index of the first attachment matching the given unique identifier within the given layer's attachments.
/// If no match was found then `-1` is returned instead.
int layer_get_attachment_index(struct layer_t *layer,
                               attachment_id_t id)
{
    for (int i = 0; i < layer->num_attachments; i++)
    {
        struct attachment_t *attachment = &layer->attachments[i];
        if (attachment->id == id)
            return i;
    }

    // if this point has been reached then no match was found
    return -1;
}

/// Add the given initialized attachment to the given layer's attachments.
/// @param layer The layer to add the given attachment to.
/// @param attachment The initialized attachment to add.
void layer_add_attachment(struct layer_t *layer,
                          struct attachment_t attachment)
{
    // insert the given attachment
    unsigned int index = layer->num_attachments++;
    layer->attachments = realloc(layer->attachments,
                                 layer->num_attachments * sizeof(struct attachment_t));

    layer->attachments[index] = attachment;
}

void layer_add_attachment_colour(struct layer_t *layer,
                                 struct colour4_t top_left,
                                 struct colour4_t top_right,
                                 struct colour4_t bottom_left,
                                 struct colour4_t bottom_right)
{
    // initialize the new colour attachment
    struct attachment_t attachment;
    attachment_init_colour(&attachment,
                           layer->next_attachment_id++,
                           top_left,
                           top_right,
                           bottom_left,
                           bottom_right);

    // add the new colour attachment
    layer_add_attachment(layer, attachment);
}

void layer_add_attachment_texture(struct layer_t *layer,
                                  struct texture_t *source,
                                  unsigned int source_index,
                                  struct uv_t bottom_left,
                                  struct uv_t top_right)
{
    // initialize the new texture attachment
    struct attachment_t attachment;
    attachment_init_texture(&attachment,
                            layer->next_attachment_id++,
                            source,
                            source_index,
                            bottom_left,
                            top_right);

    // add the new texture attachment
    layer_add_attachment(layer, attachment);
}

void layer_remove_attachment(struct layer_t *layer,
                             attachment_id_t id)
{
    // attempt to get the index of the attachment to remove
    int index = layer_get_attachment_index(layer, id);
    if (index < 0)
    {
        // the attachment could not be found, print the details and terminate
        fprintf(stderr, "LAYER ERROR: could not locate attachment %u within layer %p\n", id, layer);
        exit(EXIT_FAILURE);
    }

    // deinitialize the attachment before removing it
    struct attachment_t *attachment = &layer->attachments[index];
    attachment_deinit(attachment);

    // shuffle and reallocate the layers attachments array to remove the attachments element
    memmove(&layer->attachments[index],
            &layer->attachments[index + 1],
            (layer->num_attachments - (index + 1)) * sizeof(struct attachment_t));

    layer->num_attachments--;
    layer->attachments = realloc(layer->attachments,
                                 layer->num_attachments * sizeof(struct attachment_t));
}

struct attachment_t *layer_get_attachment(struct layer_t *layer,
                                          attachment_id_t id)
{
    int index = layer_get_attachment_index(layer, id);
    if (index >= 0)
        return &layer->attachments[index];
    else
        return NULL;
}
