#pragma once

#include "gl.h"

///
/// Meshes are used to draw a set of indexed vertices with defined components in OpenGL.
/// These components are bound to vertex attributes so they can be used within shaders.
///

// MARK: - Enumerations

/// The different types that a single mesh component's values can be.
enum mesh_component_value_type_t
{
    /// 32-bit floating point number.
    MESH_COMPONENT_F32 = 0,
};

// MARK: - Data Structures

/// A set of indexed vertices which can be drawn in OpenGL.
struct mesh_t
{
    /// The total number of indices within this mesh's vertex indices array.
    unsigned int num_indices;

    /// The unique OpenGL identifier of this mesh's vertex array.
    GLuint vertex_array_id;

    /// The unique OpenGL identifier of this mesh's vertex buffer.
    GLuint vertex_buffer_id;

    /// The unique OpenGL identifier of this mesh's vertex index buffer.
    GLuint index_buffer_id;
};

/// A single component of a single vertex within a mesh.
struct mesh_component_t
{
    /// The index of the vertex attribute that this component is bound to.
    ///
    /// It is assumed that within a set of components this value is unique.
    unsigned int attribute_index;

    /// The total number of values within this component.
    unsigned int num_values;

    /// The type of all the values within this component.
    enum mesh_component_value_type_t value_type;

    /// The size, in bytes, of the trailing padding of this component, if any.
    size_t padding;
};

// MARK: - Functions

/// Initialize the given mesh with the given vertices and vertex indices, described by the given components.
/// @param mesh The mesh to initialize.
/// @param vertices_size The total size, in bytes, of the given vertices.
/// @param vertices All the vertices of the given mesh.
/// These vertices are not drawn directly, but rather by the given indices into these vertices.
/// @param indices_size The total size, in bytes, of the given vertex indices.
/// @param indices All the vertex indices of the given mesh.
/// Each index points to a vertex within the given vertices.
/// It is assumed when drawing that these indices form triangles.
/// @param num_components The total number of components within each vertex of the given vertices.
/// @param components All the components within each vertex of the given vertices.
void mesh_init(struct mesh_t *mesh,
               size_t vertices_size,
               const void *vertices,
               size_t indices_size,
               const unsigned int *indices,
               unsigned int num_components,
               const struct mesh_component_t *components);

/// Deinitialize the given mesh, releasing all of it's allocated resources.
/// @param mesh The mesh to deinitialize.
void mesh_deinit(struct mesh_t *mesh);

/// Draw the entire contents of the given mesh.
///
/// The given mesh is drawn with the currently used program.
/// @param mesh The mesh to draw.
void mesh_draw(struct mesh_t *mesh);
