#pragma once

#include "texture.h"
#include "uv.h"

///
/// Atlas Set (AST) is a binary file format for storing several texture atlases and the sub-textures within them.
/// These sub-textures are referred to as "sprites", while the texture atlases are "atlases".
///
/// The relationship between atlases and sprites is relatively weak; atlases are unaware of their sprites,
/// and sprites only refer to their containing atlases by a unique identifier.
/// This is to simplify the retrieval process where the caller does not have to be aware of which atlas a sprite belongs to to retrieve it.
///

// MARK: - Macros

/// The maximum size of a fixed-size identifier string within an atlas set, in characters.
///
/// This includes the trailing null terminator.
#define AST_ID_MAX_SIZE (72)

// MARK: - Data Structures

/// An atlas set.
struct ast_t
{
    /// The open handle to this set's file.
    FILE *file;

    /// The width of this set's atlas array texture, in pixels.
    unsigned int atlas_width;

    /// The height of this set's atlas array texture, in pixels.
    unsigned int atlas_height;

    /// The filter used when scaling this set's atlas array texture up and down.
    enum texture_scaling_t atlas_scaling;

    /// The total number of texture atlases within this set.
    unsigned int num_atlases;

    /// All the texture atlases within this set.
    ///
    /// Allocated.
    struct ast_atlas_t
    {
        /// The width of this atlas' texture, in pixels.
        unsigned int width;

        /// The height of this atlas' texture, in pixels.
        unsigned int height;

        /// The pointer to this atlas' PNG file within the containing atlas set's file.
        long png_pointer;
    } *atlases;

    /// The total number of sprites within this set.
    unsigned int num_sprites;

    /// All the sprites within this set.
    ///
    /// Allocated.
    struct ast_sprite_t
    {
        /// The unique identifier of this sprite within the containing atlas set.
        char id[AST_ID_MAX_SIZE];

        /// The index of the atlas that this sprite belongs to within the containing atlas set.
        unsigned int atlas_index;

        /// The bottom left UV coordinates of this sprite's bounds.
        ///
        /// When this sprite is being written this is normalized to the containing atlas' texture's size.
        /// When this sprite is being read this is normalized to the containing set's atlas arrray texture's size.
        struct uv_t bottom_left;

        /// The top right UV coordinates of this sprite's bounds.
        ///
        /// When this sprite is being written this is normalized to the containing atlas' texture's size.
        /// When this sprite is being read this is normalized to the containing set's atlas arrray texture's size.
        struct uv_t top_right;

        /// The width of this sprite, in pixels.
        ///
        /// This is unused when this sprite is written, as it is calculated beforehand.
        unsigned int width;

        /// The height of this sprite, in pixels.
        ///
        /// This is unused when this sprite is written, as it is calculated beforehand.
        unsigned int height;
    } *sprites;
};

// MARK: - Functions

/// Initialize the given atlas set from the atlas set file at the given filesystem path.
///
/// If the given filesystem path is unable to be opened then the program terminates.
/// If there is no valid set file within the file at the given filesystem path then an assertion fails.
/// @param ast The set to initialize.
/// @param path The filesystem path of the set file to open.
void ast_init(struct ast_t *ast, const char *path);

/// Deinitialize the given atlas set, releasing all of its allocated resources.
/// @param ast The set to deinitialize.
void ast_deinit(struct ast_t *ast);

/// Read the atlas array texture from the given atlas set into the given texture.
///
/// The given texture is initialized with a 2D array texture containing all the atlas textures from the given set.
/// Sprites can use these textures by indexing into the array by their `atlas_index` property.
/// The atlas textures are read from disk, so this should only be called during load time.
/// During this function the given texture is initialized, so the caller is responsible for deinitializing it.
/// During this function `TEXTURE_INIT_UNIT` is activated and bound to.
/// @param ast The set to get the atlas array texture of.
/// @param texture The texture to initialize with the new atlas array texture.
void ast_get_texture(struct ast_t *ast, struct texture_t *texture);

/// Write the given atlas set contents to an atlas set file at the current cursor of the given file handle.
///
/// If the atlas index of any of the given sprites is out of bounds of the given atlases then an assertion fails.
/// During this function the cursor of the given file handle is changed.
/// @param file The file handle to write the given set contents to.
/// @param atlas_scaling The filter to use when scaling the new set's atlas array texture up and down.
/// @param num_atlases The total number of given atlases.
/// @param atlases All the atlases to write to the set file.
/// @param num_sprites The total number of given sprites.
/// @param sprites All the sprites to write to the set file.
void ast_write_contents(FILE *file,
                        enum texture_scaling_t atlas_scaling,
                        unsigned int num_atlases,
                        const struct texture_t *atlases,
                        unsigned int num_sprites,
                        const struct ast_sprite_t *sprites);
