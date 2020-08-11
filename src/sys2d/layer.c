#include "layer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// MARK: - Functions

/// Set the given layer's, and optionally its children's, dirt to the given dirt.
/// @param layer The layer to set the dirt of.
/// @param dirt The dirt to set.
/// @param recursive Whether or not the given layer's children should have their dirt recursively set to the given dirt.
void layer_set_dirt(struct layer_t *layer,
                    enum layer_dirt_t dirt,
                    bool recursive)
{
    // set the given layers dirt
    layer->dirt = dirt;

    // set the given layers childrens dirt
    if (recursive)
        for (int i = 0; i < layer->num_children; i++)
            layer_set_dirt(&layer->children[i], dirt, true);
}

/// Render the given colour attachment using the current state of the given layer, initializing the given mesh with said rendered state.
///
/// If the given attachment is not a colour attachment then an assertion fails.
/// @param attachment The colour attachment to render.
/// @param mesh The mesh to initialize.
/// @param layer The layer to use the state of to render the given attachment.
void layer_attachment_colour_render_mesh(const struct layer_attachment_t *attachment,
                                         struct mesh_t *mesh,
                                         const struct layer_t *layer)
{
    // ensure the given attachment is a colour attachment
    assert(attachment->type == LAYER_ATTACHMENT_COLOUR);

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
    struct colour4_t tl = attachment->colour_top_left;
    struct colour4_t tr = attachment->colour_top_right;
    struct colour4_t bl = attachment->colour_bottom_left;
    struct colour4_t br = attachment->colour_bottom_right;
    const float vertices[] =
    {
        0, 0, 0,    tl.r, tl.g, tl.b, tl.a, //top-left
        w, 0, 0,    tr.r, tr.g, tr.b, tr.a, //top-right
        0, h, 0,    bl.r, bl.g, bl.b, bl.a, //bottom-left
        w, h, 0,    br.r, br.g, br.b, br.a, //bottom-right
    };

    // indices
    // use two triangles to form a quad
    const unsigned int indices[] =
    {
        0, 1, 2, //top-left triangle
        1, 2, 3, //bottom-right triangle
    };

    // initialize the mesh
    mesh_init(mesh,
              sizeof(components) / sizeof(struct mesh_component_t),
              components,
              sizeof(vertices),
              vertices,
              sizeof(indices),
              indices);
}

/// Render the given texture attachment using the current state of the given layer, initializing the given mesh with said rendered state.
///
/// If the given attachment is not a texture attachment then an assertion fails.
/// @param attachment The texture attachment to render.
/// @param mesh The mesh to initialize.
/// @param layer The layer to use the state of to render the given attachment.
void layer_attachment_texture_render_mesh(const struct layer_attachment_t *attachment,
                                          struct mesh_t *mesh,
                                          const struct layer_t *layer)
{
    // ensure the given attachment is a texture attachment
    assert(attachment->type == LAYER_ATTACHMENT_TEXTURE);

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
    struct uv_t bl = attachment->texture_bottom_left;
    struct uv_t tr = attachment->texture_top_right;
    unsigned int i = attachment->texture_index;
    const float vertices[] =
    {
        0, 0, 0,    bl.u, tr.v,    i, //top-left
        w, 0, 0,    tr.u, tr.v,    i, //top-right
        0, h, 0,    bl.u, bl.v,    i, //bottom-left
        w, h, 0,    tr.u, bl.v,    i, //bottom-right
    };

    // indices
    // use two triangles to form a quad
    const unsigned int indices[] =
    {
        0, 1, 2, //top-left triangle
        1, 2, 3, //bottom-right triangle
    };

    // initialize the mesh
    mesh_init(mesh,
              sizeof(components) / sizeof(struct mesh_component_t),
              components,
              sizeof(vertices),
              vertices,
              sizeof(indices),
              indices);
}

/// Perform a render pass on the given layer and its children, rendering only when their dirt indicates to.
/// @param layer The layer to render.
void layer_render(struct layer_t *layer)
{
    // render the given layer, depending on its dirt
    enum layer_dirt_t dirt = layer->dirt;
    if (dirt & LAYER_ATTACHMENTS)
    {
        // attachments need to be re-rendered
        for (int i = 0; i < layer->num_attachments; i++)
        {
            struct layer_attachment_t *attachment = &layer->attachments[i];

            // dont need to dealloc any existing mesh as it will be immediately reused
            if (attachment->rendered_state.mesh != NULL)
                mesh_deinit(attachment->rendered_state.mesh);
            else
                attachment->rendered_state.mesh = malloc(sizeof(struct mesh_t));

            // render the current attachments mesh
            // dispatch to the appropriate mesh render function for the current attachments type
            struct mesh_t *mesh = attachment->rendered_state.mesh;
            switch (attachment->type)
            {
                case LAYER_ATTACHMENT_COLOUR:
                    layer_attachment_colour_render_mesh(attachment,
                                                        mesh,
                                                        layer);
                    break;
                case LAYER_ATTACHMENT_TEXTURE:
                    layer_attachment_texture_render_mesh(attachment,
                                                         mesh,
                                                         layer);
                    break;
            }
        }
    }
    if (dirt & LAYER_TRANSFORM)
    {
        // the transform model matrix needs to be updated
        // calculate the new matrix
        struct vector3_t absolute_anchor = vector3(layer->rendered_state.parent_size.x * layer->properties.anchor.x,
                                                   layer->rendered_state.parent_size.y * layer->properties.anchor.y,
                                                   0);

        struct vector3_t absolute_origin = vector3(-layer->properties.size.x * layer->properties.origin.x,
                                                   -layer->properties.size.y * layer->properties.origin.y,
                                                   0);

        struct matrix4_t model = layer->rendered_state.parent_transform_world;
        model = matrix4_multiply(model, matrix4_translation(absolute_origin));
        model = matrix4_multiply(model, matrix4_translation(absolute_anchor));

        // set the new matrix
        layer->rendered_state.transform_world = model;
    }

    // render the given layers children, passing any rendered state from their parent
    for (int i = 0; i < layer->num_children; i++)
    {
        struct layer_t *child = &layer->children[i];
        child->rendered_state.parent_size = layer->properties.size;
        child->rendered_state.parent_transform_world = layer->rendered_state.transform_world;
        layer_render(child);
    }

    // reset the given layers dirt to reflect that the changes have been rendered
    layer_set_dirt(layer, 0x0, false);
}

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

/// Initialize the given layer as a root layer with the given properties and attachments, without performing a render pass on it.
/// @param layer The layer to initialize.
/// @param id The unique identifier of the new layer, within its parent's children.
/// @param properties The properties of the new layer.
/// @param num_attachments The total number of given attachments.
/// @param attachments All the attachments to attach to the new layer.
/// The elements of this array are copied, so it does not need to remain accessible.
/// It is expected that the properties of each attachment are available for the entire lifetime of the new layer.
void layer_init_dirty(struct layer_t *layer,
                      layer_id_t id,
                      struct layer_properties_t properties,
                      unsigned int num_attachments,
                      const struct layer_attachment_t *attachments)
{
    // initialize the given layer
    layer->id = id;
    layer->properties = properties;
    layer->next_child_id = 0;
    layer->num_children = 0;
    layer->children = malloc(0);

    // copy the given attachments
    size_t attachments_size = num_attachments * sizeof(struct layer_attachment_t);
    layer->num_attachments = num_attachments;
    layer->attachments = malloc(attachments_size);
    memcpy(layer->attachments, attachments, attachments_size);

    // set the dirt
    layer_set_dirt(layer, LAYER_ATTACHMENTS | LAYER_TRANSFORM, true);
}

void layer_init(struct layer_t *layer,
                struct layer_properties_t properties,
                unsigned int num_attachments,
                const struct layer_attachment_t *attachments)
{
    // initialize the given layer
    layer_init_dirty(layer,
                     0,
                     properties,
                     num_attachments,
                     attachments);

    // perform the first render pass
    // set defaults for rendered parent state as it will never be set otherwise
    layer->rendered_state.parent_size = vector2_zero();
    layer->rendered_state.parent_transform_world = matrix4_identity();
    layer_render(layer);
}

void layer_deinit(struct layer_t *layer)
{
    // children
    for (int i = 0; i < layer->num_children; i++)
        layer_deinit(&layer->children[i]);
    free(layer->children);

    // meshes
    for (int i = 0; i < layer->num_attachments; i++)
    {
        struct layer_attachment_t *attachment = &layer->attachments[i];
        if (attachment->rendered_state.mesh != NULL)
        {
            mesh_deinit(attachment->rendered_state.mesh);
            free(attachment->rendered_state.mesh);
        }
    }
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
    layer_init_dirty(child_layer,
                     layer->next_child_id++,
                     properties,
                     num_attachments,
                     attachments);

    // perform the first render pass
    // the parent is rendered to allow the parents rendered state to be set on the child
    layer_render(layer);

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

void layer_add_attachment(struct layer_t *layer,
                          struct layer_attachment_t attachment)
{
    // insert the new attachment
    unsigned int index = layer->num_attachments++;
    layer->attachments = realloc(layer->attachments,
                                 layer->num_attachments * sizeof(struct layer_attachment_t));

    // copy the given attachment in
    struct layer_attachment_t *new_attachment = &layer->attachments[index];
    *new_attachment = attachment;
    new_attachment->rendered_state.mesh = NULL;

    // perform the first render pass for the new attachment
    layer_set_dirt(layer, LAYER_ATTACHMENTS, false);
    layer_render(layer);
}

void layer_remove_attachment(struct layer_t *layer,
                             unsigned int index)
{
    // deinitialize the attachments render state before removing it
    struct layer_attachment_t *attachment = &layer->attachments[index];
    if (attachment->rendered_state.mesh != NULL)
    {
        mesh_deinit(attachment->rendered_state.mesh);
        free(attachment->rendered_state.mesh);
    }

    // shuffle and reallocate the layers attachments array to remove the attachments element
    memmove(&layer->attachments[index],
            &layer->attachments[index + 1],
            (layer->num_attachments - (index + 1)) * sizeof(struct layer_attachment_t));

    layer->num_attachments--;
    layer->attachments = realloc(layer->attachments,
                                 layer->num_attachments * sizeof(struct layer_attachment_t));
}
