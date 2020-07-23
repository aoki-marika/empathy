#pragma once

///
/// Data structures and functions for working with three-dimensional vectors.
///

// MARK: - Data Structures

/// A three-dimensional vector.
struct vector3_t
{
    /// The X component of this vector.
    float x;

    /// The Y component of this vector.
    float y;

    /// The Z component of this vector.
    float z;
};

// MARK: - Functions

/// Construct and return a new 3D vector from the given components.
///
/// See `struct vector3_t` for parameter documentation.
/// @return The new 3D vector from the given components.
struct vector3_t vector3(float x, float y, float z);

/// Create and return a new 3D vector with all components set to zero.
/// @return A 3D vector with all components set to zero.
struct vector3_t vector3_zero();
