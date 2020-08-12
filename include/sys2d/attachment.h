#pragma once

#include <core/colour.h>
#include <core/texture.h>
#include <core/uv.h>
#include <core/mesh.h>
#include <core/vector.h>

///
/// Attachments define different types of visual contents for layers.
///
/// An attachment can be one of several types:
///  - Colour: The attachment draws a quad with colours in each corner, interpolating between them.
///  - Texture: The attachment draws a quad with a texture on it, sampling the texture within UV bounds.
///
/// Attachments follow the same rendering system as layers, re-rendering as little as possible and using dirt to track changes affecting rendered state.
/// See `layer.h` for further documentation on this.
///

// MARK: - Type Definitions

/// A unique identifier for an attachment, within its containing layer.
typedef unsigned int attachment_id_t;

// MARK: - Macros

/// The index of the vertex attribute that the rendered meshes of attachments bind their XYZ positions to.
///
/// XYZ coordinates are in pixels, with a top-left origin.
#define ATTACHMENT_XYZ_ATTRIBUTE_INDEX           (0)

/// The index of the vertex attribute that the rendered meshes of attachments bind their RGBA colours to.
///
/// RGBA components are normalized from `0` to `1`.
#define ATTACHMENT_RGBA_ATTRIBUTE_INDEX          (1)

/// The index of the vertex attribute that the rendered meshes of attachments bind their texture UV coordinates to.
///
/// UV components are normalized from `0` to `1`, with a bottom-left origin.
#define ATTACHMENT_UV_ATTRIBUTE_INDEX            (2)

/// The index of the vertex attribute that the rendered meshes of attachments bind their array texture index to.
///
/// Array texture indices are in elements.
#define ATTACHMENT_TEXTURE_INDEX_ATTRIBUTE_INDEX (3)

// MARK: - Data Structures

/// A single attachment.
struct attachment_t
{
    /// The unique identifier of this attachment within its containing layer.
    attachment_id_t id;

    /// The type of this attachment.
    enum attachment_type_t
    {
        /// The attachment draws colours on the four corners of a quad, interpolating between them.
        ATTACHMENT_COLOUR = 0x0,

        /// The attachment draws a texture on a quad, sampling it using UV bounds.
        ATTACHMENT_TEXTURE = 0x1,
    } type;

    /// All the properties of this attachment affected by changes which have occurred within this attachment since the last render pass.
    enum attachment_dirt_t
    {
        // None.
        ATTACHMENT_NONE = 0 << 0,

        /// Mesh.
        ATTACHMENT_MESH = 1 << 0,
    } dirt;

    /// The backing colour attachment of this attachment, if any.
    ///
    /// This is only used if `type` is `ATTACHMENT_COLOUR`.
    struct attachment_colour_t
    {
        /// The colour of the top-left corner of this attachment.
        struct colour4_t top_left;

        /// The colour of the top-right corner of this attachment.
        struct colour4_t top_right;

        /// The colour of the bottom-left corner of this attachment.
        struct colour4_t bottom_left;

        /// The colour of the bottom-right corner of this attachment.
        struct colour4_t bottom_right;
    } colour;

    /// The backing texture attachment of this attachment, if any.
    ///
    /// This is only used if `type` is `ATTACHMENT_TEXTURE`.
    struct attachment_texture_t
    {
        /// The texture of which this attachment samples.
        ///
        /// The lifetime of this texture is handled by the creator of this attachment.
        struct texture_t *source;

        /// The index, within this attachment's array texture, of the element that this attachment samples.
        ///
        /// This is only used if `texture` is of type `TEXTURE_2D_ARRAY`.
        unsigned int source_index;

        /// The bottom-left UV coordinates of the bounds that this attachment samples its texture from.
        struct uv_t bottom_left;

        /// The top-right UV coordinates of the bounds that this attachment samples its texture from.
        struct uv_t top_right;
    } texture;

    /// The last rendered state of this attachment.
    struct attachment_rendered_state_t
    {
        /// The mesh of the attachment, if any.
        ///
        /// Only size is accounted for in this mesh; positional or otherwise properties must be handled by the drawer.
        /// Allocated.
        struct mesh_t *mesh;
    } rendered_state;
};

// MARK: - Functions

/// Initialize the given attachment as a colour attachment with the given properties.
/// @param attachment The attachment to initialize.
/// @param id The unique identifier of the new attachment within its containing layer.
/// See `attachment_colour_t` for property documentation.
void attachment_init_colour(struct attachment_t *attachment,
                            attachment_id_t id,
                            struct colour4_t top_left,
                            struct colour4_t top_right,
                            struct colour4_t bottom_left,
                            struct colour4_t bottom_right);

/// Initialize the given attachment as a texture attachment with the given properties.
/// @param attachment The attachment to initialize.
/// @param id The unique identifier of the new attachment within its containing layer.
/// See `attachment_texture_t` for property documentation.
void attachment_init_texture(struct attachment_t *attachment,
                             attachment_id_t id,
                             struct texture_t *source,
                             unsigned int source_index,
                             struct uv_t bottom_left,
                             struct uv_t top_right);

/// Deinitialize the given attachment, releasing all of its allocated resources.
/// @param attachment The attachment to deinitialize.
void attachment_deinit(struct attachment_t *attachment);

/// Perform a render pass on the given attachment, re-rendering any properties that have changed since the last render pass.
///
/// This must be called before the given attachment is drawn when it is initialized or when its properties change.
/// Attachments on their own will not perform a render pass, which will leave the rendered state empty.
/// @param attachment The attachment to render.
/// @param size The size to render the given attachment at, in pixels.
void attachment_render(struct attachment_t *attachment,
                       struct vector2_t size);
