#pragma once

///
/// Data structures and functions for working with UV texture coordinates.
///

// MARK: - Data Structures

/// A set of UV texture coordinates.
struct uv_t
{
    /// The normalized U coordinate of these coordinates, from `0` (left) to `1` (right).
    float u;

    /// The normalized V coordinate of these coordinates, from `0` (bottom) to `1` (top).
    float v;
};

// MARK: - Functions

/// Construct and return a new set of UV texture coordinates from the given coordinates.
///
/// See `struct uv_t` for parameter documentation.
/// @return The new UV texture coordinates from the given coordinates.
struct uv_t uv(float u, float v);

/// Create and return a new set of UV texture coordinates with all coordinates set to zero.
/// @return A set of UV texture coordinates with all coordinates set to zero.
struct uv_t uv_zero();
