#pragma once

///
/// Data structures and functions for working with two and three-dimensional vectors.
///

// MARK: - Data Structures

/// A two-dimensional vector.
struct vector2_t
{
    /// The X component of this vector.
    float x;

    /// The Y component of this vector.
    float y;
};

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

/// Construct and return a new 2D vector from the given components.
///
/// See `struct vector2_t` for parameter documentation.
/// @return The new 2D vector from the given components.
struct vector2_t vector2(float x, float y);

/// Create and return a new 2D vector with all components set to zero.
/// @return A 2D vector with all components set to zero.
struct vector2_t vector2_zero();

/// Construct and return a new 3D vector from the given components.
///
/// See `struct vector3_t` for parameter documentation.
/// @return The new 3D vector from the given components.
struct vector3_t vector3(float x, float y, float z);

/// Create and return a new 3D vector with all components set to zero.
/// @return A 3D vector with all components set to zero.
struct vector3_t vector3_zero();
