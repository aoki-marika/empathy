#include "bin.h"

#include <stdint.h>

// MARK: - Macros

/// Cast the given value to the given type and write it to the current cursor of the given file handle.
/// @param value The value to cast.
/// @param type The type to cast the given value to.
/// @param file The file handle to write the given value to once it has been casted.
#define BIN_WRITE_CAST(value, type, file) \
    type write_value = (type)value; \
    fwrite(&write_value, sizeof(write_value), 1, file);

/// Read and return the value of the given type from the current cursor of the given file handle, casted to the given type.
/// @param read_type The type to read from the given file handle.
/// @param cast_type The type to cast the read value to before returning it.
/// @param file The file handle to read the value from.
/// @return The value read from the given file handle, casted to the given type.
#define BIN_READ_CAST(read_type, cast_type, file) \
    read_type value; \
    fread(&value, sizeof(value), 1, file); \
    return (cast_type)value;

// MARK: - Functions

void bin_write_s8(int value, FILE *file)
{
    BIN_WRITE_CAST(value, int8_t, file);
}

void bin_write_s16(int value, FILE *file)
{
    BIN_WRITE_CAST(value, int16_t, file);
}

void bin_write_s32(int value, FILE *file)
{
    BIN_WRITE_CAST(value, int32_t, file);
}

void bin_write_u8(unsigned int value, FILE *file)
{
    BIN_WRITE_CAST(value, uint8_t, file);
}

void bin_write_u16(unsigned int value, FILE *file)
{
    BIN_WRITE_CAST(value, uint16_t, file);
}

void bin_write_u32(unsigned int value, FILE *file)
{
    BIN_WRITE_CAST(value, uint32_t, file);
}

void bin_write_f32(float value, FILE *file)
{
    BIN_WRITE_CAST(value, float, file);
}

int bin_read_s8(FILE *file)
{
    BIN_READ_CAST(int8_t, int, file);
}

int bin_read_s16(FILE *file)
{
    BIN_READ_CAST(int16_t, int, file);
}

int bin_read_s32(FILE *file)
{
    BIN_READ_CAST(int32_t, int, file);
}

unsigned int bin_read_u8(FILE *file)
{
    BIN_READ_CAST(uint8_t, unsigned int, file);
}

unsigned int bin_read_u16(FILE *file)
{
    BIN_READ_CAST(uint16_t, unsigned int, file);
}

unsigned int bin_read_u32(FILE *file)
{
    BIN_READ_CAST(uint32_t, unsigned int, file);
}

float bin_read_f32(FILE *file)
{
    BIN_READ_CAST(float, float, file);
}
