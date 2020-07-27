#pragma once

///
/// Data structures and utilities related to working with colours.
///

// MARK: - Data Structures

/// An RGBA colour.
struct colour4_t
{
    /// The normalized red component of this colour.
    float r;

    /// The normalized green component of this colour.
    float g;

    /// The normalized blue component of this colour.
    float b;

    /// The normalized alpha component of this colour.
    float a;
};

// MARK: - Functions

/// Construct and return a new RGBA colour from the given components.
///
/// See `struct colour4_t` for parameter documentation.
/// @return The new RGBA colour from the given components.
struct colour4_t colour4(float r, float g, float b, float a);
