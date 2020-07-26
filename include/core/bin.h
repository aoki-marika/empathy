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

void bin_write_f32(float value, FILE *file);

///
/// Read the given specific sized types as generic types from the given files.
/// @param file The file to read the value from.
/// @return The value from the given file.
///

int bin_read_s8(FILE *file);
int bin_read_s16(FILE *file);
int bin_read_s32(FILE *file);

unsigned int bin_read_u8(FILE *file);
unsigned int bin_read_u16(FILE *file);
unsigned int bin_read_u32(FILE *file);

float bin_read_f32(FILE *file);
