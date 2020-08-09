#include "layer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// MARK: - Functions

/// Get the index of the first child layer matching the given unique identifier within the given parent's children.
/// @param child_id The unique identifier of the child layer to get the index of.
/// @param parent The layer containing the layer to get the index of.
/// @return The index of the first child layer matching the given unique identifier within the given parent's children.
/// If no matches were found then `-1` is returned instead.
int layer_get_child_index(layer_id_t child_id,
                          struct layer_t *parent)
{
    // attempt to return the index of the first layer matching the given id within the given parents children
    for (int i = 0; i < parent->num_children; i++)
    {
        struct layer_t *child = &parent->children[i];
        if (child->id == child_id)
            return i;
    }

    // if this point has been reached then no match was found
    return -1;
}

void layer_init_root(struct layer_t *layer,
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

void layer_init_child(layer_id_t *child_id,
                      struct layer_t *parent,
                      struct layer_properties_t properties,
                      unsigned int num_attachments,
                      const struct layer_attachment_t *attachments)
{
    // insert the new child layer
    unsigned int layer_index = parent->num_children;
    parent->num_children++;
    parent->children = realloc(parent->children,
                               parent->num_children * sizeof(struct layer_t));

    struct layer_t *layer = &parent->children[layer_index];

    // initialize the new child layer
    layer_init_root(layer,
                    properties,
                    num_attachments,
                    attachments);
    layer->id = parent->next_child_id++;

    // set the given child id pointers value, if there is one
    if (child_id != NULL)
        *child_id = layer->id;
}

void layer_deinit(struct layer_t *layer)
{
    for (int i = 0; i < layer->num_children; i++)
        layer_deinit(&layer->children[i]);

    free(layer->children);
    free(layer->attachments);
}

struct layer_t *layer_get_child(layer_id_t child_id,
                                struct layer_t *parent)
{
    int index = layer_get_child_index(child_id, parent);
    if (index >= 0)
        return &parent->children[index];
    else
        return NULL;
}

void layer_set_properties(struct layer_t *layer,
                          struct layer_properties_t properties)
{
    layer->properties = properties;
}

void layer_remove(layer_id_t child_id,
                  struct layer_t *parent)
{
    // attempt to get the index of the layer to remove
    int index = layer_get_child_index(child_id, parent);
    if (index < 0)
    {
        // the child layer could not be found, print the details and terminate
        fprintf(stderr, "LAYER ERROR: could not locate child layer %u within layer %p\n", child_id, parent);
        exit(EXIT_FAILURE);
    }

    // deinit the child layer before removing it
    struct layer_t *layer = &parent->children[index];
    layer_deinit(layer);

    // shuffle and reallocate the parents children array to remove the child layers element
    memmove(&parent->children[index],
            &parent->children[index + 1],
            (parent->num_children - (index + 1)) * sizeof(struct layer_t));

    parent->num_children--;
    parent->children = realloc(parent->children,
                               parent->num_children * sizeof(struct layer_t));
}
