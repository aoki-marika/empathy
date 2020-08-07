#include "ast.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "bin.h"
#include "png.h"

// MARK: - Macros

/// The size of the header within an atlas set file, in bytes.
///
///  - ASCII `AST\0` signature.
///  - U16 atlas array texture width.
///  - U16 atlas array texture height.
///  - U8 atlas array texture scaling.
///  - U8 `0x00` padding. (x3)
///  - U32 atlas count.
///  - U32 atlases pointer.
///  - U32 sprite count.
///  - U32 sprites pointer.
#define AST_HEADER_SIZE (28)

/// The size of an atlas within an atlas set file, in bytes.
///
///  - U16 texture width.
///  - U16 texture height.
///  - U32 PNG pointer.
#define AST_ATLAS_SIZE (8)

/// The size of a sprite within an atlas set file, in bytes.
///
///  - 72-byte null-terminated ASCII identifier.
///  - U8 containing atlas index.
///  - U8 `0x00` padding. (x3)
///  - F32 bottom-left U coordinate.
///  - F32 bottom-left V coordinate.
///  - F32 top-right U coordinate.
///  - F32 top-right V coordinate.
///  - U16 pixel width.
///  - U16 pixel height.
#define AST_SPRITE_SIZE (AST_ID_MAX_SIZE + 24)

// MARK: - Functions

void ast_init(struct ast_t *ast, const char *path)
{
    // open the given file
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        // the given path could not be opened, print the details and terminate
        fprintf(stderr, "AST ERROR: unable to open ast file at \"%s\" (%s)\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // read the header
    // signature
    assert(fgetc(file) == 'A');
    assert(fgetc(file) == 'S');
    assert(fgetc(file) == 'T');
    assert(fgetc(file) == 0x0);

    // atlas array texture width, height, and scaling
    unsigned int atlas_width = bin_read_u16(file);
    unsigned int atlas_height = bin_read_u16(file);
    enum texture_scaling_t atlas_scaling = bin_read_u8(file);
    assert(bin_read_u8(file) == 0x0);
    assert(bin_read_u8(file) == 0x0);
    assert(bin_read_u8(file) == 0x0);

    // atlas count and pointer
    unsigned int num_atlases = bin_read_u32(file);
    unsigned int atlases_pointer = bin_read_u32(file);

    // sprite count and pointer
    unsigned int num_sprites = bin_read_u32(file);
    unsigned int sprites_pointer = bin_read_u32(file);

    // read the atlases
    struct ast_atlas_t *atlases = malloc(num_atlases * sizeof(struct ast_atlas_t));
    for (int i = 0; i < num_atlases; i++)
    {
        // read the current atlas
        fseek(file, atlases_pointer + (i * AST_ATLAS_SIZE), SEEK_SET);
        unsigned int width = bin_read_u16(file);
        unsigned int height = bin_read_u16(file);
        long png_pointer = (long)bin_read_u32(file);

        // initialize the current atlas
        struct ast_atlas_t *atlas = &atlases[i];
        atlas->width = width;
        atlas->height = height;
        atlas->png_pointer = png_pointer;
    }

    // read the sprites
    struct ast_sprite_t *sprites = malloc(num_sprites * sizeof(struct ast_sprite_t));
    for (int i = 0; i < num_sprites; i++)
    {
        // read the current sprite
        fseek(file, sprites_pointer + (i * AST_SPRITE_SIZE), SEEK_SET);
        char id[AST_ID_MAX_SIZE];
        fread(id, AST_ID_MAX_SIZE, 1, file);
        unsigned int atlas_index = bin_read_u8(file);
        assert(bin_read_u8(file) == 0x0);
        assert(bin_read_u8(file) == 0x0);
        assert(bin_read_u8(file) == 0x0);
        float bottom_left_u = bin_read_f32(file);
        float bottom_left_v = bin_read_f32(file);
        float top_right_u = bin_read_f32(file);
        float top_right_v = bin_read_f32(file);
        unsigned int width = bin_read_u16(file);
        unsigned int height = bin_read_u16(file);

        // initialize the current sprite
        struct ast_sprite_t *sprite = &sprites[i];
        memcpy(sprite->id, id, AST_ID_MAX_SIZE * sizeof(char));
        sprite->atlas_index = atlas_index;
        sprite->bottom_left.u = bottom_left_u;
        sprite->bottom_left.v = bottom_left_v;
        sprite->top_right.u = top_right_u;
        sprite->top_right.v = top_right_v;
        sprite->width = width;
        sprite->height = height;
    }

    // initialize the given set
    ast->file = file;
    ast->atlas_width = atlas_width;
    ast->atlas_height = atlas_height;
    ast->atlas_scaling = atlas_scaling;
    ast->num_atlases = num_atlases;
    ast->atlases = atlases;
    ast->num_sprites = num_sprites;
    ast->sprites = sprites;
}

void ast_deinit(struct ast_t *ast)
{
    free(ast->sprites);
    free(ast->atlases);
    fclose(ast->file);
}

void ast_get_texture(struct ast_t *ast,
                     struct texture_t *texture)
{
    // read all the pngs
    struct png_t pngs[ast->num_atlases];
    for (int i = 0; i < ast->num_atlases; i++)
    {
        struct ast_atlas_t *atlas = &ast->atlases[i];
        fseek(ast->file, atlas->png_pointer, SEEK_SET);
        png_init_file(&pngs[i], ast->file);
    }

    // initialize the given texture
    texture_init_png_array(texture,
                           ast->atlas_width,
                           ast->atlas_height,
                           ast->atlas_scaling,
                           ast->num_atlases,
                           pngs);

    // deinitialize all the pngs as theyve now been uploaded
    for (int i = 0; i < ast->num_atlases; i++)
        png_deinit(&pngs[i]);
}

void ast_write_contents(FILE *file,
                        enum texture_scaling_t atlas_scaling,
                        unsigned int num_atlases,
                        const struct texture_t *atlases,
                        unsigned int num_sprites,
                        const struct ast_sprite_t *sprites)
{
    // calculate the atlas array texture size
    unsigned int atlas_width = 0, atlas_height = 0;
    for (int i = 0; i < num_atlases; i++)
    {
        const struct texture_t *atlas = &atlases[i];
        if (atlas->width > atlas_width)
            atlas_width = atlas->width;
        if (atlas->height > atlas_height)
            atlas_height = atlas->height;
    }

    // calculate fixed pointers
    uint32_t header_pointer = (uint32_t)ftell(file);
    uint32_t atlases_pointer = header_pointer + AST_HEADER_SIZE;
    uint32_t sprites_pointer = atlases_pointer + (num_atlases * AST_ATLAS_SIZE);
    uint32_t stream_pointer = sprites_pointer + (num_sprites * AST_SPRITE_SIZE);

    // open the data stream
    // this stream is used to write dynamically-sized data, such as pngs,
    // in a block below statically-sized data
    // this allows pointers to be easily calculated while writing the file
    char *stream;
    size_t stream_size = 0;
    FILE *stream_file = open_memstream(&stream, &stream_size);

    // write the header
    fprintf(file, "AST");
    bin_write_u8(0x0, file);
    bin_write_u16(atlas_width, file);
    bin_write_u16(atlas_height, file);
    bin_write_u8(atlas_scaling, file);
    bin_write_u8(0x0, file);
    bin_write_u8(0x0, file);
    bin_write_u8(0x0, file);
    bin_write_u32(num_atlases, file);
    bin_write_u32(atlases_pointer, file);
    bin_write_u32(num_sprites, file);
    bin_write_u32(sprites_pointer, file);

    // write the atlases
    for (int i = 0; i < num_atlases; i++)
    {
        // seek to this atlas in the file and get its source
        fseek(file, atlases_pointer + (i * AST_ATLAS_SIZE), SEEK_SET);
        const struct texture_t *atlas = &atlases[i];

        // write the atlas
        uint32_t png_pointer = stream_pointer + ftell(stream_file);
        bin_write_u16(atlas->width, file);
        bin_write_u16(atlas->height, file);
        bin_write_u32(png_pointer, file);

        // write the png
        struct png_t png;
        png_init_texture(&png, atlas);
        png_write(&png, stream_file);
        png_deinit(&png);
    }

    // write the sprites
    for (int i = 0; i < num_sprites; i++)
    {
        // seek to the sprite in the file and get its source
        fseek(file, sprites_pointer + (i * AST_SPRITE_SIZE), SEEK_SET);
        const struct ast_sprite_t *sprite = &sprites[i];

        // write the sprite
        fwrite(sprite->id, sizeof(char), AST_ID_MAX_SIZE, file);
        bin_write_u8(sprite->atlas_index, file);
        bin_write_u8(0x0, file);
        bin_write_u8(0x0, file);
        bin_write_u8(0x0, file);

        // normalize the uv coordinates to the atlas array textures size before writing them
        // this allows the reader to not have to do any work
        assert(sprite->atlas_index < num_atlases);
        const struct texture_t *atlas = &atlases[i];
        float u_multiplier = (float)atlas->width / (float)atlas_width;
        float v_multiplier = (float)atlas->height / (float)atlas_height;
        float bl_u = sprite->bottom_left.u * u_multiplier;
        float bl_v = sprite->bottom_left.v * v_multiplier;
        float tr_u = sprite->top_right.u * u_multiplier;
        float tr_v = sprite->top_right.v * v_multiplier;

        bin_write_f32(bl_u, file);
        bin_write_f32(bl_v, file);
        bin_write_f32(tr_u, file);
        bin_write_f32(tr_v, file);

        // calculate and write the pixel size
        bin_write_u16(atlas->width * (sprite->top_right.u - sprite->bottom_left.u), file);
        bin_write_u16(atlas->height * (sprite->top_right.v - sprite->bottom_left.v), file);
    }

    // write and close the data stream
    fflush(stream_file);
    fwrite(stream, stream_size, 1, file);
    fclose(stream_file);
}
