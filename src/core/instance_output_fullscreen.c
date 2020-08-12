#include "instance_output_fullscreen.h"

// MARK: - Functions

/// The initialization function for a fullscreen instance output's backing.
///
/// See `instance_init_function_t` for further documentation.
void instance_output_fullscreen_output_init(struct instance_t *instance,
                                            void *data)
{
    struct instance_output_fullscreen_t *output = (struct instance_output_fullscreen_t *)data;

    // create the shader program
    {
        // minimal vertex shader with normalized xy and uv coordinates
        const char *vertex_source = \
            "#version 330 core\n"
            "\n"
            "layout(location=0) in vec2 vertex_position;\n"
            "layout(location=1) in vec2 vertex_uv;\n"
            "\n"
            "out vec2 uv;\n"
            "\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(vertex_position.xy, 0, 1);\n"
            "   uv = vertex_uv;\n"
            "}\n";

        // minimal fragment shader which samples a sampler with vertex uv coordinates
        const char *fragment_source = \
            "#version 330 core\n"
            "\n"
            "uniform sampler2D sampler;\n"
            "\n"
            "in vec2 uv;\n"
            "\n"
            "void main()\n"
            "{\n"
            "   gl_FragColor = texture(sampler, uv);"
            "}\n";

        shader_init(&output->shaders[0], SHADER_VERTEX, vertex_source);
        shader_init(&output->shaders[1], SHADER_FRAGMENT, fragment_source);
        program_init(&output->program, sizeof(output->shaders) / sizeof(struct shader_t), output->shaders);

        // set the constant uniforms
        program_use(&output->program);
        program_set_sampler2D(&output->program,
                              "sampler",
                              INSTANCE_OUTPUT_FULLSCREEN_FRAMEBUFFER_UNIT);
    }

    // create the mesh
    {
        // attribute 0 xy floats, attribute 1 uv floats
        const struct mesh_component_t components[] =
        {
            {
                .attribute_index = 0,
                .num_values = 2,
                .value_type = MESH_COMPONENT_F32,
                .padding = 0,
            },
            {
                .attribute_index = 1,
                .num_values = 2,
                .value_type = MESH_COMPONENT_F32,
                .padding = 0,
            },
        };

        // vertices for each normalized corner of the quad
        const float vertices[] =
        {
            -1,  1,    0, 1, //top-left
             1,  1,    1, 1, //top-right
            -1, -1,    0, 0, //bottom-left
             1, -1,    1, 0, //bottom-right
        };

        // two triangles to form a quad
        const unsigned int indices[] =
        {
            0, 1, 2, //top-left triangle
            1, 2, 3, //bottom-right triangle
        };

        mesh_init(&output->mesh,
                  sizeof(components) / sizeof(struct mesh_component_t),
                  components,
                  sizeof(vertices),
                  vertices,
                  sizeof(indices),
                  indices);
    }
}

/// The deinitialization function for a fullscreen instance output's backing.
///
/// See `instance_deinit_function_t` for further documentation.
void instance_output_fullscreen_output_deinit(struct instance_t *instance,
                                              void *data)
{
    struct instance_output_fullscreen_t *output = (struct instance_output_fullscreen_t *)data;

    // deinit
    mesh_deinit(&output->mesh);
    program_deinit(&output->program);
    for (int i = 0; i < sizeof(output->shaders) / sizeof(struct shader_t); i++)
        shader_deinit(&output->shaders[i]);
}

/// The render function for a fullscreen instance output's backing.
///
/// See `instance_output_render_function_t` for further documentation.
void instance_output_fullscreen_output_render(struct instance_t *instance,
                                              void *data,
                                              struct framebuffer_t *framebuffer)
{
    struct instance_output_fullscreen_t *output = (struct instance_output_fullscreen_t *)data;

    // render the final framebuffer mesh
    program_use(&output->program);
    texture_bind(&instance->program.framebuffer.texture,
                 INSTANCE_OUTPUT_FULLSCREEN_FRAMEBUFFER_UNIT);
    mesh_draw(&output->mesh);
}

void instance_output_fullscreen_init(struct instance_output_fullscreen_t *output)
{
    // initialize the backing output
    struct instance_output_t *backing = &output->backing;
    backing->data = output;
    backing->init = instance_output_fullscreen_output_init;
    backing->deinit = instance_output_fullscreen_output_deinit;
    backing->render = instance_output_fullscreen_output_render;
}

void instance_output_fullscreen_deinit(struct instance_output_fullscreen_t *output)
{
}
