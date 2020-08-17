#include "attachment.h"

#include <stdlib.h>

// MARK: - Functions

/// Initialize the given mesh with the given colour attachment properties at the given size.
/// @param properties The colour attachment properties to use.
/// @param size The size of the new mesh, in pixels.
/// @param mesh The mesh to initialize.
void attachment_colour_render_mesh(struct attachment_colour_properties_t *properties,
                                   struct vector2_t size,
                                   struct mesh_t *mesh)
{
    // create the mesh
    // components
    static const struct mesh_component_t components[] =
    {
        {
            .attribute_index = ATTACHMENT_XYZ_ATTRIBUTE_INDEX,
            .num_values = 3,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
        {
            .attribute_index = ATTACHMENT_RGBA_ATTRIBUTE_INDEX,
            .num_values = 4,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
    };

    // vertices
    // use temporary shorthands to make the initializer shorter
    float w = size.x;
    float h = size.y;
    struct colour4_t tl = properties->top_left;
    struct colour4_t tr = properties->top_right;
    struct colour4_t bl = properties->bottom_left;
    struct colour4_t br = properties->bottom_right;
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

/// Initialize the given mesh with the given texture attachment properties at the given size.
/// @param properties The texture attachment properties to use.
/// @param size The size of the new mesh, in pixels.
/// @param mesh The mesh to initialize.
void attachment_texture_render_mesh(struct attachment_texture_properties_t *properties,
                                    struct vector2_t size,
                                    struct mesh_t *mesh)
{
    // create the mesh
    // components
    // texture index is always passed regardless of whether or not it is used, to simplify mesh creation
    static const struct mesh_component_t components[] =
    {
        {
            .attribute_index = ATTACHMENT_XYZ_ATTRIBUTE_INDEX,
            .num_values = 3,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
        {
            .attribute_index = ATTACHMENT_UV_ATTRIBUTE_INDEX,
            .num_values = 2,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
        {
            .attribute_index = ATTACHMENT_TEXTURE_INDEX_ATTRIBUTE_INDEX,
            .num_values = 1,
            .value_type = MESH_COMPONENT_F32,
            .padding = 0,
        },
    };

    // vertices
    // use temporary shorthands to make the initializer shorter
    float w = size.x;
    float h = size.y;
    struct uv_t bl = properties->bottom_left;
    struct uv_t tr = properties->top_right;
    unsigned int si = properties->source_index;
    const float vertices[] =
    {
        0, 0, 0,    bl.u, tr.v,    si, //top-left
        w, 0, 0,    tr.u, tr.v,    si, //top-right
        0, h, 0,    bl.u, bl.v,    si, //bottom-left
        w, h, 0,    tr.u, bl.v,    si, //bottom-right
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

/// Initialize the given attachment with default property values.
/// @param attachment The attachment to initialize.
void attachment_init_dirty(struct attachment_t *attachment)
{
    // initialize the given attachment
    // attachment
    attachment->dirt = ATTACHMENT_MESH;

    // colour properties
    attachment->colour_properties.top_left = colour4(1, 1, 1, 1);
    attachment->colour_properties.top_right = colour4(1, 1, 1, 1);
    attachment->colour_properties.bottom_left = colour4(1, 1, 1, 1);
    attachment->colour_properties.bottom_right = colour4(1, 1, 1, 1);

    // texture properties
    attachment->texture_properties.source = NULL;
    attachment->texture_properties.source_index = 0;
    attachment->texture_properties.bottom_left = uv(0, 0);
    attachment->texture_properties.top_right = uv(1, 1);

    // render result
    attachment->render_result.mesh = NULL;
}

void attachment_init_colour(struct attachment_t *attachment,
                            struct colour4_t top_left,
                            struct colour4_t top_right,
                            struct colour4_t bottom_left,
                            struct colour4_t bottom_right)
{
    // initialize the given attachment
    attachment_init_dirty(attachment);
    attachment->type = ATTACHMENT_COLOUR;
    attachment->colour_properties.top_left = top_left;
    attachment->colour_properties.top_right = top_right;
    attachment->colour_properties.bottom_left = bottom_left;
    attachment->colour_properties.bottom_right = bottom_right;
}

void attachment_init_texture(struct attachment_t *attachment,
                             struct texture_t *source,
                             unsigned int source_index,
                             struct uv_t bottom_left,
                             struct uv_t top_right)
{
    // initialize the given attachment
    attachment_init_dirty(attachment);
    attachment->type = ATTACHMENT_TEXTURE;
    attachment->texture_properties.source = source;
    attachment->texture_properties.source_index = source_index;
    attachment->texture_properties.bottom_left = bottom_left;
    attachment->texture_properties.top_right = top_right;
}

void attachment_deinit(struct attachment_t *attachment)
{
    struct mesh_t *mesh = attachment->render_result.mesh;
    if (mesh != NULL)
    {
        mesh_deinit(mesh);
        free(mesh);
    }
}

void attachment_render(struct attachment_t *attachment,
                       struct vector2_t size)
{
    // render the given attachment, depending on its dirt
    enum attachment_dirt_t dirt = attachment->dirt;
    if (dirt & ATTACHMENT_MESH)
    {
        // the mesh needs to be re-initialized
        // get the mesh to initialize
        // if the mesh is already allocated then it only needs to be reinitialized
        struct mesh_t **mesh = &attachment->render_result.mesh;
        if (*mesh != NULL)
            mesh_deinit(*mesh);
        else
            *mesh = malloc(sizeof(struct mesh_t));

        // initialize the mesh with the current state of the given attachment
        switch (attachment->type)
        {
            case ATTACHMENT_COLOUR:
                attachment_colour_render_mesh(&attachment->colour_properties,
                                              size,
                                              *mesh);
                break;
            case ATTACHMENT_TEXTURE:
                attachment_texture_render_mesh(&attachment->texture_properties,
                                               size,
                                               *mesh);
                break;
        }
    }

    // reset the given attachments dirt, as it has now been applied
    attachment->dirt = ATTACHMENT_NONE;
}

void attachment_set_colour_top_left(struct attachment_t *attachment,
                                    struct colour4_t value)
{
    attachment->colour_properties.top_left = value;
    attachment->dirt |= ATTACHMENT_MESH;
}

void attachment_set_colour_top_right(struct attachment_t *attachment,
                                     struct colour4_t value)
{
    attachment->colour_properties.top_right = value;
    attachment->dirt |= ATTACHMENT_MESH;
}

void attachment_set_colour_bottom_left(struct attachment_t *attachment,
                                       struct colour4_t value)
{
    attachment->colour_properties.bottom_left = value;
    attachment->dirt |= ATTACHMENT_MESH;
}

void attachment_set_colour_bottom_right(struct attachment_t *attachment,
                                        struct colour4_t value)
{
    attachment->colour_properties.bottom_right = value;
    attachment->dirt |= ATTACHMENT_MESH;
}

void attachment_set_texture_source(struct attachment_t *attachment,
                                   struct texture_t *value)
{
    attachment->texture_properties.source = value;
}

void attachment_set_texture_source_index(struct attachment_t *attachment,
                                         unsigned int value)
{
    attachment->texture_properties.source_index = value;
    attachment->dirt |= ATTACHMENT_MESH;
}

void attachment_set_texture_bottom_left(struct attachment_t *attachment,
                                        struct uv_t value)
{
    attachment->texture_properties.bottom_left = value;
    attachment->dirt |= ATTACHMENT_MESH;
}

void attachment_set_texture_top_right(struct attachment_t *attachment,
                                      struct uv_t value)
{
    attachment->texture_properties.top_right = value;
    attachment->dirt |= ATTACHMENT_MESH;
}
