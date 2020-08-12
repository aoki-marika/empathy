#include "drawer.h"

#include <core/vector.h>
#include <core/matrix.h>

// MARK: - Functions

/// Initialize the given program, attaching the given vertex and fragment shaders to it.
/// @param program The program to initialize.
/// @param vertex The vertex shader to attach.
/// @param vertex The fragment shader to attach.
void drawer_program_init(struct program_t *program,
                         const struct shader_t *vertex,
                         const struct shader_t *fragment)
{
    struct shader_t shaders[2] =
    {
        *vertex,
        *fragment,
    };

    program_init(program, 2, shaders);
}

/// Initialize all the shaders and shader programs of the given drawer.
/// @param drawer The drawer to initialize the shaders and shader programs of.
void drawer_init_shaders(struct drawer_t *drawer)
{
    // define the shader sources
    // the vertex shader is shared among all programs and sends vertex components to the fragment shader
    // fragment shaders are then implemented by type to avoid any sort of branching logic
    // projection and view are pre-multiplied when passed into the vertex shader to avoid any unnecessary calculations on the gpu
    static const char *vertex_source = \
        "#version 330 core\n"
        "\n"
        "layout(location=0) in vec3 vertex_xyz;\n"
        "layout(location=1) in vec4 vertex_rgba;\n"
        "layout(location=2) in vec2 vertex_uv;\n"
        "layout(location=3) in float vertex_texture_index;\n"
        "\n"
        "uniform mat4 model;\n"
        "uniform mat4 projection_view;\n"
        "\n"
        "out vec3 xyz;\n"
        "out vec4 rgba;\n"
        "out vec2 uv;\n"
        "out float texture_index;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection_view * model * vec4(vertex_xyz, 1.0);\n"
        "    xyz = vertex_xyz;\n"
        "    rgba = vertex_rgba;\n"
        "    uv = vertex_uv;\n"
        "    texture_index = vertex_texture_index;\n"
        "}\n";

    static const char *fragment_colour_source = \
        "#version 330 core\n"
        "\n"
        "in vec4 rgba;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = rgba;\n"
        "}\n";

    static const char *fragment_texture_2d_source = \
        "#version 330 core\n"
        "\n"
        "uniform sampler2D sampler;\n"
        "\n"
        "in vec2 uv;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = texture(sampler, uv);\n"
        "}\n";

    static const char *fragment_texture_2d_array_source = \
        "#version 330 core\n"
        "\n"
        "uniform sampler2DArray sampler;\n"
        "\n"
        "in vec2 uv;\n"
        "in float texture_index;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = texture(sampler, vec3(uv, texture_index));\n"
        "}\n";

    // initialize the shaders
    shader_init(&drawer->vertex,
                SHADER_VERTEX,
                vertex_source);

    shader_init(&drawer->fragment_colour,
                SHADER_FRAGMENT,
                fragment_colour_source);

    shader_init(&drawer->fragment_texture_2d,
                SHADER_FRAGMENT,
                fragment_texture_2d_source);

    shader_init(&drawer->fragment_texture_2d_array,
                SHADER_FRAGMENT,
                fragment_texture_2d_array_source);

    // initialize the shader programs
    drawer_program_init(&drawer->program_colour,
                        &drawer->vertex,
                        &drawer->fragment_colour);

    drawer_program_init(&drawer->program_texture_2d,
                        &drawer->vertex,
                        &drawer->fragment_texture_2d);

    drawer_program_init(&drawer->program_texture_2d_array,
                        &drawer->vertex,
                        &drawer->fragment_texture_2d_array);
}

void drawer_init(struct drawer_t *drawer,
                 unsigned int draw_width,
                 unsigned int draw_height)
{
    // initialize the shaders and shader programs
    drawer_init_shaders(drawer);

    // calculate the projection and view matrices
    // orthographic projection matrices always place 0,0 at the center of the screen,
    // so use the view matrix to offset this to the top-left
    float dw = (float)draw_width, dh = (float)draw_height;
    struct matrix4_t projection = matrix4_orthographic(-dw / 2,
                                                       dw / 2,
                                                       -dh / 2,
                                                       dh / 2,
                                                       0,
                                                       1);

    struct matrix4_t view = matrix4_translation(vector3(-dw / 2,
                                                        -dh / 2,
                                                        0));

    struct matrix4_t projection_view = matrix4_multiply(projection, view);

    // set shader program constants
    // colour attachment
    program_use(&drawer->program_colour);
    program_set_mat4(&drawer->program_colour, "projection_view", projection_view);

    // 2d texture attachment
    program_use(&drawer->program_texture_2d);
    program_set_mat4(&drawer->program_texture_2d, "projection_view", projection_view);
    program_set_sampler2D(&drawer->program_texture_2d, "sampler", DRAWER_UNIT);

    // 2d array texture attachment
    program_use(&drawer->program_texture_2d_array);
    program_set_mat4(&drawer->program_texture_2d_array, "projection_view", projection_view);
    program_set_sampler2DArray(&drawer->program_texture_2d_array, "sampler", DRAWER_UNIT);
}

void drawer_deinit(struct drawer_t *drawer)
{
    program_deinit(&drawer->program_texture_2d_array);
    program_deinit(&drawer->program_texture_2d);
    program_deinit(&drawer->program_colour);

    shader_deinit(&drawer->fragment_texture_2d_array);
    shader_deinit(&drawer->fragment_texture_2d);
    shader_deinit(&drawer->fragment_colour);
    shader_deinit(&drawer->vertex);
}
void layer_draw(const struct layer_t *layer,
                struct drawer_t *drawer)
{
    // if there are no attachments then the layer cannot be drawn
    if (layer->num_attachments > 0)
    {
        // get the attachment to draw
        // TODO: proper attachment selection
        const struct layer_attachment_t *attachment = &layer->attachments[0];

        // get the program to use to draw the attachment
        struct program_t *program;
        switch (attachment->type)
        {
            case LAYER_ATTACHMENT_COLOUR:
                program = &drawer->program_colour;
                break;
            case LAYER_ATTACHMENT_TEXTURE:
                switch (attachment->texture->type)
                {
                    case TEXTURE_2D:
                        program = &drawer->program_texture_2d;
                        break;
                    case TEXTURE_2D_ARRAY:
                        program = &drawer->program_texture_2d_array;
                        break;
                }
                break;
        }

        // configure the program
        program_use(program);
        program_set_mat4(program, "model", layer->rendered_state.transform_world);
        if (attachment->type == LAYER_ATTACHMENT_TEXTURE)
            texture_bind(attachment->texture, DRAWER_UNIT);

        // draw the attachment
        mesh_draw(attachment->rendered_state.mesh);
    }

    // draw the given layers children
    for (int i = 0; i < layer->num_children; i++)
        layer_draw(&layer->children[i], drawer);
}
