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
///  - U32 atlas count.
///  - U32 atlases pointer.
///  - U32 sprite count.
///  - U32 sprites pointer.
#define AST_HEADER_SIZE 20

/// The size of an atlas within an atlas set file, in bytes.
///
///  - U8 texture scaling.
///  - U8 `0x00` padding.
///  - U16 texture width.
///  - U16 texture height.
///  - U8 `0x00` padding. (x2)
///  - U32 PNG pointer.
#define AST_ATLAS_SIZE 12

/// The size of a sprite within an atlas set file, in bytes.
///
///  - 48-byte ASCII identifier.
///  - U16 containing atlas index.
///  - U8 `0x00` padding. (x2)
///  - F32 bottom-left U coordinate.
///  - F32 bottom-left V coordinate.
///  - F32 top-right U coordinate.
///  - F32 top-right V coordinate.
///  - U16 pixel width.
///  - U16 pixel height.
#define AST_SPRITE_SIZE 72

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
        fseek(file, atlases_pointer + (i * 8), SEEK_SET);
        enum texture_scaling_t scaling = bin_read_u8(file);
        assert(bin_read_u8(file) == 0x0);
        unsigned int width = bin_read_u16(file);
        unsigned int height = bin_read_u16(file);
        assert(bin_read_u8(file) == 0x0);
        assert(bin_read_u8(file) == 0x0);
        long png_pointer = (long)bin_read_u32(file);

        // initialzie the current atlas
        struct ast_atlas_t *atlas = &atlases[i];
        atlas->width = width;
        atlas->height = height;
        atlas->scaling = scaling;
        atlas->png_pointer = png_pointer;
    }

    // read the sprites
    struct ast_sprite_t *sprites = malloc(num_sprites * sizeof(struct ast_sprite_t));
    for (int i = 0; i < num_sprites; i++)
    {
        // read the current sprite
        fseek(file, sprites_pointer + (i * 68), SEEK_SET);
        char id[AST_ID_MAX_SIZE];
        fread(id, AST_ID_MAX_SIZE, 1, file);
        unsigned int atlas_index = bin_read_u16(file);
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
        memcpy(sprite->id, id, AST_ID_MAX_SIZE);
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

void ast_atlas_read(struct ast_t *ast, unsigned int index, struct texture_t *texture)
{
    // get the given atlas, ensuring the index is valid
    assert(index < ast->num_atlases);
    struct ast_atlas_t *atlas = &ast->atlases[index];

    // read the png
    struct png_t png;
    fseek(ast->file, atlas->png_pointer, SEEK_SET);
    png_init_file(&png, ast->file);

    // initialize the given texture
    texture_init_png(texture, atlas->scaling, &png);

    // deinitialize the png as its now loaded into the texture
    png_deinit(&png);
}

void ast_write_contents(FILE *file,
                        unsigned int num_atlases,
                        struct texture_t *atlases,
                        unsigned int num_sprites,
                        struct ast_sprite_t *sprites)
{
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
    bin_write_u32(num_atlases, file);
    bin_write_u32(atlases_pointer, file);
    bin_write_u32(num_sprites, file);
    bin_write_u32(sprites_pointer, file);

    // write the atlases
    for (int i = 0; i < num_atlases; i++)
    {
        // seek to this atlas in the file and get its source
        fseek(file, atlases_pointer + (i * AST_ATLAS_SIZE), SEEK_SET);
        struct texture_t *atlas = &atlases[i];

        // write the atlas
        uint32_t png_pointer = stream_pointer + ftell(stream_file);
        bin_write_u8(atlas->scaling, file);
        bin_write_u8(0x0, file);
        bin_write_u16(atlas->width, file);
        bin_write_u16(atlas->height, file);
        bin_write_u8(0x0, file);
        bin_write_u8(0x0, file);
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
        struct ast_sprite_t *sprite = &sprites[i];

        // write the sprite
        fwrite(sprite->id, sizeof(char), AST_ID_MAX_SIZE, file);
        bin_write_u16(sprite->atlas_index, file);
        bin_write_u8(0x0, file);
        bin_write_u8(0x0, file);
        bin_write_f32(sprite->bottom_left.u, file);
        bin_write_f32(sprite->bottom_left.v, file);
        bin_write_f32(sprite->top_right.u, file);
        bin_write_f32(sprite->top_right.v, file);

        // calculate and write the pixel size
        assert(sprite->atlas_index < num_atlases);
        struct texture_t *atlas = &atlases[i];
        bin_write_u16(atlas->width * (sprite->top_right.u - sprite->bottom_left.u), file);
        bin_write_u16(atlas->height * (sprite->top_right.v - sprite->bottom_left.v), file);
    }

    // write and close the data stream
    fflush(stream_file);
    fwrite(stream, stream_size, 1, file);
    fclose(stream_file);
}
