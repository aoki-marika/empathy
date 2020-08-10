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

/// Initialize the render state of the given colour attachment, using the current state of the given layer.
/// @param attachment The colour attachment to initialize the render state of.
/// @param layer The layer to use the state of to initialize the given colour attachment's render state.
void layer_attachment_render_state_init_colour(struct layer_attachment_t *attachment,
                                               const struct layer_t *layer)
{
    // create the mesh
    // components
    const struct mesh_component_t components[] =
    {
        {
            .attribute_index = LAYER_ATTACHMENT_XYZ_ATTRIBUTE_INDEX,
            .num_values = 3,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
        {
            .attribute_index = LAYER_ATTACHMENT_RGBA_ATTRIBUTE_INDEX,
            .num_values = 4,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
    };

    // vertices
    // use temporary shorthands to make the initializer shorter
    float w = layer->properties.size.x;
    float h = layer->properties.size.y;
    float x = -layer->properties.origin.x * w;
    float y = -layer->properties.origin.y * h;
    struct colour4_t tl = attachment->colour_top_left;
    struct colour4_t tr = attachment->colour_top_right;
    struct colour4_t bl = attachment->colour_bottom_left;
    struct colour4_t br = attachment->colour_bottom_right;
    const float vertices[] =
    {
            x,     y, 0,    tl.r, tl.g, tl.b, tl.a, //top-left
        x + w,     y, 0,    tr.r, tr.g, tr.b, tr.a, //top-right
            x, y + h, 0,    bl.r, bl.g, bl.b, bl.a, //bottom-left
        x + w, y + h, 0,    br.r, br.g, br.b, br.a, //bottom-right
    };

    // indices
    // use two triangles to form a quad
    const unsigned int indices[] =
    {
        0, 1, 2, //top-left triangle
        1, 2, 3, //bottom-right triangle
    };

    // initialize the mesh
    mesh_init(&attachment->render_state.mesh,
              sizeof(components) / sizeof(struct mesh_component_t),
              components,
              sizeof(vertices),
              vertices,
              sizeof(indices),
              indices);
}

/// Initialize the render state of the given texture attachment, using the current state of the given layer.
/// @param attachment The texture attachment to initialize the render state of.
/// @param layer The layer to use the state of to initialize the given texture attachment's render state.
void layer_attachment_render_state_init_texture(struct layer_attachment_t *attachment,
                                                const struct layer_t *layer)
{
    // create the mesh
    // components
    // texture index is always passed regardless of whether or not it is used, to simplify mesh creation
    const struct mesh_component_t components[] =
    {
        {
            .attribute_index = LAYER_ATTACHMENT_XYZ_ATTRIBUTE_INDEX,
            .num_values = 3,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
        {
            .attribute_index = LAYER_ATTACHMENT_UV_ATTRIBUTE_INDEX,
            .num_values = 2,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
        {
            .attribute_index = LAYER_ATTACHMENT_TEXTURE_INDEX_ATTRIBUTE_INDEX,
            .num_values = 1,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
    };

    // vertices
    // use temporary shorthands to make the initializer shorter
    float w = layer->properties.size.x;
    float h = layer->properties.size.y;
    float x = -layer->properties.origin.x * w;
    float y = -layer->properties.origin.y * h;
    struct uv_t bl = attachment->texture_bottom_left;
    struct uv_t tr = attachment->texture_top_right;
    unsigned int i = attachment->texture_index;
    const float vertices[] =
    {
            x,     y, 0,    bl.u, tr.v,    i, //top-left
        x + w,     y, 0,    tr.u, tr.v,    i, //top-right
            x, y + h, 0,    bl.u, bl.v,    i, //bottom-left
        x + w, y + h, 0,    tr.u, bl.v,    i, //bottom-right
    };

    // indices
    // use two triangles to form a quad
    const unsigned int indices[] =
    {
        0, 1, 2, //top-left triangle
        1, 2, 3, //bottom-right triangle
    };

    // initialize the mesh
    mesh_init(&attachment->render_state.mesh,
              sizeof(components) / sizeof(struct mesh_component_t),
              components,
              sizeof(vertices),
              vertices,
              sizeof(indices),
              indices);
}

/// Initialize the render state of the given attachment, using the current state of the given layer.
/// @param attachment The attachment to initialize the render state of.
/// @param layer The layer to use the state of to initialize the given attachment's render state.
void layer_attachment_render_state_init(struct layer_attachment_t *attachment,
                                        const struct layer_t *layer)
{
    // dispatch to the appropriate initializer for the given attachments type
    switch (attachment->type)
    {
        case LAYER_ATTACHMENT_COLOUR:
            layer_attachment_render_state_init_colour(attachment, layer);
            break;
        case LAYER_ATTACHMENT_TEXTURE:
            layer_attachment_render_state_init_texture(attachment, layer);
            break;
    }
}

/// Deinitialize the render state of the given attachment, releasing all of its allocated resources.
///
/// This must be called on an attachment before `layer_attachment_render_state_init(...)` can be called again.
/// @param attachment The attachment to deinitialize the render state of.
void layer_attachment_render_state_deinit(struct layer_attachment_t *attachment)
{
    mesh_deinit(&attachment->render_state.mesh);
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
    layer->properties = properties;

    // copy the given attachments
    size_t attachments_size = num_attachments * sizeof(struct layer_attachment_t);
    layer->num_attachments = num_attachments;
    layer->attachments = malloc(attachments_size);
    memcpy(layer->attachments, attachments, attachments_size);

    // initialize all the attachment render states
    for (int i = 0; i < num_attachments; i++)
        layer_attachment_render_state_init(&layer->attachments[i], layer);
}

void layer_deinit(struct layer_t *layer)
{
    for (int i = 0; i < layer->num_children; i++)
        layer_deinit(&layer->children[i]);
    free(layer->children);

    for (int i = 0; i < layer->num_attachments; i++)
        layer_attachment_render_state_deinit(&layer->attachments[i]);
    free(layer->attachments);
}

void layer_add_child(struct layer_t *layer,
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

struct layer_t *layer_get_child(struct layer_t *layer,
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
    // set the given layers properties
    layer->properties = properties;

    // reinitialize all the given layers attachments render states with the new properties
    for (int i = 0; i < layer->num_attachments; i++)
    {
        struct layer_attachment_t *attachment = &layer->attachments[i];
        layer_attachment_render_state_deinit(attachment);
        layer_attachment_render_state_init(attachment, layer);
    }
}

void layer_add_attachment(struct layer_t *layer,
                          struct layer_attachment_t attachment)
{
    // insert the new attachment
    unsigned int index = layer->num_attachments++;
    layer->attachments = realloc(layer->attachments,
                                 layer->num_attachments * sizeof(struct layer_attachment_t));

    // copy the given attachment in and initialize its render state
    struct layer_attachment_t *new_attachment = &layer->attachments[index];
    *new_attachment = attachment;
    layer_attachment_render_state_init(new_attachment, layer);
}

void layer_remove_attachment(struct layer_t *layer,
                             unsigned int index)
{
    // deinitialize the attachments render state before removing it
    struct layer_attachment_t *attachment = &layer->attachments[index];
    layer_attachment_render_state_deinit(attachment);

    // shuffle and reallocate the layers attachments array to remove the attachments element
    memmove(&layer->attachments[index],
            &layer->attachments[index + 1],
            (layer->num_attachments - (index + 1)) * sizeof(struct layer_attachment_t));

    layer->num_attachments--;
    layer->attachments = realloc(layer->attachments,
                                 layer->num_attachments * sizeof(struct layer_attachment_t));
}
