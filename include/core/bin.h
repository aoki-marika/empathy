#pragma once

#include <stdio.h>

///
/// Binary file reading and writing utilities.
///

// MARK: - Functions

///
/// Write given generic types as specific sized types to the given files.
/// @param value The value to write.
/// @param file The file to write the given value to.
///

void bin_write_s8(int value, FILE *file);
void bin_write_s16(int value, FILE *file);
void bin_write_s32(int value, FILE *file);

void bin_write_u8(unsigned int value, FILE *file);
void bin_write_u16(unsigned int value, FILE *file);
void bin_write_u32(unsigned int value, FILE *file);
