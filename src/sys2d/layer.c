#include "layer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MARK: - Functions

/// Get the index of the first child layer matching the given unique identifier within the given layer's children.
/// @param child_id The unique identifier of the child layer to get the index of.
/// @param layer The layer containing the layer to get the index of.
/// @return The index of the first child layer matching the given unique identifier within the given layer's children.
/// If no matches were found then `-1` is returned instead.
int layer_get_child_index(struct layer_t *layer,
                          layer_id_t child_id)
{
    // attempt to return the index of the first layer matching the given id within the given layers children
    for (int i = 0; i < layer->num_children; i++)
    {
        struct layer_t *child_layer = &layer->children[i];
        if (child_layer->id == child_id)
            return i;
    }

    // if this point has been reached then no match was found
    return -1;
}

void layer_init(struct layer_t *layer,
                struct layer_properties_t properties,
                unsigned int num_attachments,
                const struct layer_attachment_t *attachments)
{
    // initialize the given layer
    layer->id = 0;
    layer->next_child_id = 0;
    layer->num_children = 0;
    layer->children = malloc(0);
    layer_set_properties(layer, properties);

    // copy the given attachments
    size_t attachments_size = num_attachments * sizeof(struct layer_attachment_t);
    layer->num_attachments = num_attachments;
    layer->attachments = malloc(attachments_size);
    memcpy(layer->attachments, attachments, attachments_size);
}

void layer_deinit(struct layer_t *layer)
{
    for (int i = 0; i < layer->num_children; i++)
        layer_deinit(&layer->children[i]);

    free(layer->children);
    free(layer->attachments);
}

void layer_add(struct layer_t *layer,
               layer_id_t *child_id,
               struct layer_properties_t properties,
               unsigned int num_attachments,
               const struct layer_attachment_t *attachments)
{
    // insert the new child layer
    unsigned int child_index = layer->num_children++;
    layer->children = realloc(layer->children,
                              layer->num_children * sizeof(struct layer_t));

    struct layer_t *child_layer = &layer->children[child_index];

    // initialize the new child layer
    layer_init(child_layer,
               properties,
               num_attachments,
               attachments);
    child_layer->id = layer->next_child_id++;

    // set the given child id pointers value, if there is one
    if (child_id != NULL)
        *child_id = child_layer->id;
}

void layer_remove(struct layer_t *layer,
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

    // deinit the child layer before removing it
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

struct layer_t *layer_get(struct layer_t *layer,
                          layer_id_t child_id)
{
    int child_index = layer_get_child_index(layer, child_id);
    if (child_index >= 0)
        return &layer->children[child_index];
    else
        return NULL;
}

void layer_set_properties(struct layer_t *layer,
                          struct layer_properties_t properties)
{
    layer->properties = properties;
}
