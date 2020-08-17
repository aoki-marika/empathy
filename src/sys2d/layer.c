#include "layer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
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
            layer_add_dirt(layer->children[i], dirt, true);
}

/// Initialize the given layer as a root layer with the given properties.
/// @param layer The layer to initialize.
/// @param parent_layer The parent layer containing the new layer within its children, if any.
/// See `layer_properties_t` for property documentation.
void layer_init_dirty(struct layer_t *layer,
                      const struct layer_t *parent_layer,
                      struct vector2_t anchor,
                      struct vector2_t origin,
                      struct vector2_t position,
                      struct vector2_t size,
                      struct vector2_t scale,
                      struct vector2_t shear,
                      float rotation)
{
    // initialize the given layer
    // layer
    layer->dirt = LAYER_TRANSFORM;

    // properties
    layer->properties.anchor = anchor;
    layer->properties.origin = origin;
    layer->properties.position = position;
    layer->properties.size = size;
    layer->properties.scale = scale;
    layer->properties.shear = shear;
    layer->properties.rotation = rotation;

    // render result
    layer->render_result.parent_size = vector2_zero();
    layer->render_result.parent_transform_world = matrix4_identity();
    layer->render_result.transform_world = matrix4_identity();

    // attachments
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
                     vector2_zero(),
                     vector2_zero(),
                     vector2_zero(),
                     size,
                     vector2(1, 1),
                     vector2_zero(),
                     0);
}

void layer_deinit(struct layer_t *layer)
{
    // children
    for (int i = 0; i < layer->num_children; i++)
    {
        struct layer_t *child_layer = layer->children[i];
        layer_deinit(child_layer);
        free(child_layer);
    }
    free(layer->children);

    // attachments
    for (int i = 0; i < layer->num_attachments; i++)
    {
        struct attachment_t *attachment = layer->attachments[i];
        attachment_deinit(attachment);
        free(attachment);
    }
    free(layer->attachments);
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

        struct vector3_t position = vector3(layer->properties.position.x,
                                            layer->properties.position.y,
                                            0);

        struct vector3_t scale = vector3(layer->properties.scale.x,
                                         layer->properties.scale.y,
                                         0);

        struct vector3_t shear = vector3(layer->properties.shear.x,
                                         layer->properties.shear.y,
                                         0);

        struct vector3_t rotation_radians = vector3(0,
                                                    0,
                                                    layer->properties.rotation * M_PI / 180);

        struct matrix4_t model = layer->render_result.parent_transform_world;
        model = matrix4_multiply(model, matrix4_translation(absolute_anchor));
        model = matrix4_multiply(model, matrix4_rotation(rotation_radians));
        model = matrix4_multiply(model, matrix4_translation(position));
        model = matrix4_multiply(model, matrix4_scaling(scale));
        model = matrix4_multiply(model, matrix4_translation(absolute_origin));
        model = matrix4_multiply(model, matrix4_shearing(shear));
        layer->render_result.transform_world = model;
    }

    // perform a render pass on the given layers attachments
    for (int i = 0; i < layer->num_attachments; i++)
    {
        struct attachment_t *attachment = layer->attachments[i];
        attachment_render(attachment, layer->properties.size);
    }

    // perform a render pass on the given layers children
    for (int i = 0; i < layer->num_children; i++)
    {
        struct layer_t *child_layer = layer->children[i];

        // pass any new render results from the parent
        if (dirt & LAYER_TRANSFORM || child_layer->dirt & LAYER_TRANSFORM)
        {
            child_layer->render_result.parent_size = layer->properties.size;
            child_layer->render_result.parent_transform_world = layer->render_result.transform_world;
        }

        layer_render(child_layer);
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

void layer_set_position(struct layer_t *layer,
                        struct vector2_t value)
{
    layer->properties.position = value;
    layer_add_dirt(layer, LAYER_TRANSFORM, true);
}

void layer_set_size(struct layer_t *layer,
                    struct vector2_t value)
{
    layer->properties.size = value;
    layer_add_dirt(layer, LAYER_TRANSFORM, true);

    // attachments need to re-render their mesh for size changes
    for (int i = 0; i < layer->num_attachments; i++)
        layer->attachments[i]->dirt |= ATTACHMENT_MESH;
}

void layer_set_scale(struct layer_t *layer,
                     struct vector2_t value)
{
    layer->properties.scale = value;
    layer_add_dirt(layer, LAYER_TRANSFORM, true);
}

void layer_set_shear(struct layer_t *layer,
                     struct vector2_t value)
{
    layer->properties.shear = value;
    layer_add_dirt(layer, LAYER_TRANSFORM, true);
}

void layer_set_rotation(struct layer_t *layer,
                        float value)
{
    layer->properties.rotation = value;
    layer_add_dirt(layer, LAYER_TRANSFORM, true);
}

// MARK: Children

/// Get the index of the given child layer within the given layer's children, if any.
/// @param layer The layer containing the given child layer.
/// @param child_layer The child layer to get the index of.
/// @return The index of the given child layer within the given layer's children.
/// If the given child layer is not within the given layer's children then `-1` is returned instead.
int layer_get_child_index(const struct layer_t *layer,
                          const struct layer_t *child_layer)
{
    for (int i = 0; i < layer->num_children; i++)
        if (layer->children[i] == child_layer)
            return i;

    // if this point has been reached then no match was found
    return -1;
}

struct layer_t *layer_add_child(struct layer_t *layer,
                                struct vector2_t anchor,
                                struct vector2_t origin,
                                struct vector2_t position,
                                struct vector2_t size,
                                struct vector2_t scale,
                                struct vector2_t shear,
                                float rotation)
{
    // initialize the new child layer
    struct layer_t *child_layer = malloc(sizeof(struct layer_t));
    layer_init_dirty(child_layer,
                     layer,
                     anchor,
                     origin,
                     position,
                     size,
                     scale,
                     shear,
                     rotation);

    // insert and return the new child layer
    unsigned int child_index = layer->num_children++;
    layer->children = realloc(layer->children,
                              layer->num_children * sizeof(struct layer_t));

    layer->children[child_index] = child_layer;
    return child_layer;
}

void layer_remove_child(struct layer_t *layer,
                        const struct layer_t *child_layer)
{
    // attempt to get the index of the child layer to remove
    int child_index = layer_get_child_index(layer, child_layer);
    if (child_index < 0)
    {
        // the child layer could not be found, print the details and terminate
        fprintf(stderr, "LAYER ERROR: could not locate child layer %p within layer %p\n", child_layer, layer);
        exit(EXIT_FAILURE);
    }

    // deinitialize the child layer before removing it
    struct layer_t *layer_child_layer = layer->children[child_index];
    layer_deinit(layer_child_layer);
    free(layer_child_layer);

    // shuffle and reallocate the layers children array to remove the child layers element
    memmove(&layer->children[child_index],
            &layer->children[child_index + 1],
            (layer->num_children - (child_index + 1)) * sizeof(struct layer_t));

    layer->num_children--;
    layer->children = realloc(layer->children,
                              layer->num_children * sizeof(struct layer_t));
}

// MARK: Attachment

/// Get the index of the given attachment within the given layer's attachments, if any.
/// @param layer The layer containing the given attachment.
/// @param attachment The attachment to get the index of.
/// @return The index of the given attachment within the given layer's attachments.
/// If the given attachment is not within the given layer's attachments then `-1` is returned instead.
int layer_get_attachment_index(const struct layer_t *layer,
                               const struct attachment_t *attachment)
{
    for (int i = 0; i < layer->num_attachments; i++)
        if (layer->attachments[i] == attachment)
            return i;

    // if this point has been reached then no match was found
    return -1;
}

/// Add the given initialized attachment to the given layer's attachments.
/// @param layer The layer to add the given attachment to.
/// @param attachment The initialized attachment to add.
/// It is expected that this attachment is allocated, and it will be freed once it is removed from the given layer.
void layer_add_attachment(struct layer_t *layer,
                          struct attachment_t *attachment)
{
    // insert the given attachment
    unsigned int index = layer->num_attachments++;
    layer->attachments = realloc(layer->attachments,
                                 layer->num_attachments * sizeof(struct attachment_t));

    layer->attachments[index] = attachment;
}

struct attachment_t *layer_add_attachment_colour(struct layer_t *layer,
                                                 struct colour4_t top_left,
                                                 struct colour4_t top_right,
                                                 struct colour4_t bottom_left,
                                                 struct colour4_t bottom_right)
{
    // initialize the new colour attachment
    struct attachment_t *attachment = malloc(sizeof(struct attachment_t));
    attachment_init_colour(attachment,
                           top_left,
                           top_right,
                           bottom_left,
                           bottom_right);

    // add and return the new colour attachment
    layer_add_attachment(layer, attachment);
    return attachment;
}

struct attachment_t *layer_add_attachment_texture(struct layer_t *layer,
                                                  struct texture_t *source,
                                                  unsigned int source_index,
                                                  struct uv_t bottom_left,
                                                  struct uv_t top_right)
{
    // initialize the new texture attachment
    struct attachment_t *attachment = malloc(sizeof(struct attachment_t));
    attachment_init_texture(attachment,
                            source,
                            source_index,
                            bottom_left,
                            top_right);

    // add and return the new texture attachment
    layer_add_attachment(layer, attachment);
    return attachment;
}

void layer_remove_attachment(struct layer_t *layer,
                             const struct attachment_t *attachment)
{
    // attempt to get the index of the attachment to remove
    int index = layer_get_attachment_index(layer, attachment);
    if (index < 0)
    {
        // the attachment could not be found, print the details and terminate
        fprintf(stderr, "LAYER ERROR: could not locate attachment %p within layer %p\n", attachment, layer);
        exit(EXIT_FAILURE);
    }

    // deinitialize the attachment before removing it
    struct attachment_t *layer_attachment = layer->attachments[index];
    attachment_deinit(layer_attachment);
    free(layer_attachment);

    // shuffle and reallocate the layers attachments array to remove the attachments element
    memmove(&layer->attachments[index],
            &layer->attachments[index + 1],
            (layer->num_attachments - (index + 1)) * sizeof(struct attachment_t));

    layer->num_attachments--;
    layer->attachments = realloc(layer->attachments,
                                 layer->num_attachments * sizeof(struct attachment_t));
}
