#include "bin.h"

#include <stdint.h>

// MARK: - Macros

/// Cast the given value to the given type and write it to the given file.
/// @param value The value to cast.
/// @param type The type to cast the given value to.
/// @param file The file to write the given value to once it has been casted.
#define BIN_WRITE_CAST(value, type, file) \
    type write_value = (type)value; \
    fwrite(&write_value, sizeof(write_value), 1, file);

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
