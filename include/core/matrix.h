#pragma once

#include "vector.h"

///
/// Data structures and functions for working with 4x4 matrices.
///

// MARK: - Data Structures

/// A 4x4 matrix.
struct matrix4_t
{
    /// All the elements of this matrix.
    ///
    /// Uses column-major indices.
    float elements[4][4];
};

// MARK: - Functions

/// Construct and return a new 4x4 matrix from the given column-major ordered elements.
///
/// `e[column][row]`
/// @return The new 4x4 matrix from the given elements.
struct matrix4_t matrix4(float e00, float e10, float e20, float e30,
                         float e01, float e11, float e21, float e31,
                         float e02, float e12, float e22, float e32,
                         float e03, float e13, float e23, float e33);

/// Create and return a new 4x4 matrix with all elements set to zero.
/// @return A 4x4 matrix with all elements set to zero.
struct matrix4_t matrix4_zero();

/// Create and return a new 4x4 identity matrix.
/// @return A 4x4 identity matrix.
struct matrix4_t matrix4_identity();

/// Multiply the two given 4x4 matrices, returning the result.
/// @param left The left-hand-side of the operation.
/// @param right The right-hand-side of the operation.
/// @return The result of multiplying the two given 4x4 matrices.
struct matrix4_t matrix4_multiply(const struct matrix4_t left,
                                  const struct matrix4_t right);

/// Create and return a new orthographic projection matrix from the given parameters.
/// @param left The coordinate of the left side of the projection.
/// @param right The coordinate of the right side of the projection.
/// @param top The coordinate of the top side of the projection.
/// @param bottom The coordinate of the bottom side of the projection.
/// @param near The near clipping plane of the projection.
/// @param far The far clipping plane of the projection.
/// @return A 4x4 orthographic projection matrix from the given parameters.
struct matrix4_t matrix4_orthographic(float left,
                                      float right,
                                      float top,
                                      float bottom,
                                      float near,
                                      float far);

/// Create and return a new translation matrix with the given 3D offset.
/// @param offset The offset of the translation in 3D space.
/// @return A 4x4 translation matrix of the given offset.
struct matrix4_t matrix4_translation(struct vector3_t offset);
