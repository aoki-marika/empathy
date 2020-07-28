#pragma once

#include "gl.h"

///
/// Meshes are a set of indexed vertices which can be drawn within a graphics context.
///
/// "Indexed vertices" refers to vertices being defined separately from the vertices that are drawn.
/// In one array single vertices are defined, and in another the vertices which are drawn refer to indices within these vertices.
/// This is a huge performance gain in large meshes as vertices are never defined twice, much reducing the vertex count.
///
/// To be usable in all cases, meshes must have their vertex components defined.
/// These components each represent a single "component" of each vertex; XYZ position, RGBA colour, UV coordinates, etc.
/// Each component contains a number of "values", each representing one value of the component; X component of RGB, G component of RGBA, etc.
/// Meshes are unaware of what these components are, they are only interested in the layout of the bytes.
/// When drawing a mesh, each of it's components for each vertex are bound to an indexed vertex attribute, which can then be used by a shader program.
///

// MARK: - Data Structures

/// A set of indexed vertices which can be drawn within a graphics context.
struct mesh_t
{
    /// The unique OpenGL identifier of this mesh's vertex array.
    GLuint vertex_array_id;

    /// The unique OpenGL identifier of this mesh's vertex buffer.
    GLuint vertex_buffer_id;

    /// The unique OpenGL identifier of this mesh's vertex index buffer.
    GLuint index_buffer_id;

    /// The total number of indices within this mesh's vertex indices array.
    unsigned int num_indices;
};

/// A single component of each vertex within a mesh.
struct mesh_component_t
{
    /// The index of the vertex attribute that this component is bound to.
    ///
    /// It is assumed that within a set of components this value is unique.
    unsigned int attribute_index;

    /// The total number of values within this component.
    unsigned int num_values;

    /// The type of all the values within this component.
    enum mesh_component_value_type_t
    {
        /// 32-bit floating point number.
        MESH_COMPONENT_F32 = 0,
    } value_type;

    /// The size, in bytes, of the trailing padding of this component, if any.
    size_t padding;
};

// MARK: - Functions

/// Initialize the given mesh with the given vertices and vertex indices, described by the given components.
/// @param mesh The mesh to initialize.
/// @param vertices_size The total size, in bytes, of the given vertices.
/// @param vertices All the vertices of the new mesh.
/// These vertices are not drawn directly, but rather by the given indices into these vertices.
/// @param indices_size The total size, in bytes, of the given vertex indices.
/// @param indices All the vertex indices of the new mesh.
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

/// Draw the entire contents of the given mesh to the current graphics context.
///
/// During this function the given mesh's vertex array is bound.
/// @param mesh The mesh to draw.
void mesh_draw(struct mesh_t *mesh);
