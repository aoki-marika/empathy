#include "mesh.h"

// MARK: - Functions

void mesh_init(struct mesh_t *mesh,
               size_t vertices_size,
               const void *vertices,
               size_t indices_size,
               const unsigned int *indices,
               unsigned int num_components,
               const struct mesh_component_t *components)
{
    // create the vertex array
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    // create the vertex buffer
    GLuint vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    // create the index buffer
    GLuint index_buffer_id;
    glGenBuffers(1, &index_buffer_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    // apply the given components
    // a first pass needs to be done to calculate the stride
    // since many values are reused in the second pass they are cached
    GLenum component_gl_value_types[num_components];
    GLsizei component_sizes[num_components];
    GLsizei vertex_size = 0;
    for (int i = 0; i < num_components; i++)
    {
        const struct mesh_component_t *component = &components[i];

        // get the opengl value type and size of the current component
        GLenum gl_value_type;
        GLsizei value_size;
        switch (component->value_type)
        {
            case MESH_COMPONENT_F32:
                gl_value_type = GL_FLOAT;
                value_size = sizeof(float);
                break;
        }

        // calculate and set the size of the current component
        GLsizei size = (component->num_values * value_size) + component->padding;
        component_gl_value_types[i] = gl_value_type;
        component_sizes[i] = size;
        vertex_size += size;
    }

    // a second pass needs to be done to configure the vertex attributes
    void *component_pointer = 0x0;
    for (int i = 0; i < num_components; i++)
    {
        const struct mesh_component_t *component = &components[i];

        // configure the vertex attribute
        glEnableVertexAttribArray(component->attribute_index);
        glVertexAttribPointer(
            component->attribute_index, //index
            component->num_values, //size
            component_gl_value_types[i], //type
            GL_FALSE, //normalized
            vertex_size, //stride
            component_pointer //pointer
        );

        // increment the offset for the next component
        component_pointer += component_sizes[i];
    }

    // initialize the mesh
    mesh->vertex_array_id = vertex_array_id;
    mesh->vertex_buffer_id = vertex_buffer_id;
    mesh->index_buffer_id = index_buffer_id;
    mesh->num_indices = indices_size / sizeof(unsigned int);
}

void mesh_deinit(struct mesh_t *mesh)
{
    glDeleteBuffers(1, &mesh->vertex_buffer_id);
    glDeleteBuffers(1, &mesh->index_buffer_id);
    glDeleteVertexArrays(1, &mesh->vertex_array_id);
}

void mesh_draw(struct mesh_t *mesh)
{
    // bind and draw all the vertices within the given mesh
    glBindVertexArray(mesh->vertex_array_id);
    glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);
}
