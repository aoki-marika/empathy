#pragma once

///
/// Data structures and utilities related to working with colours.
///

// MARK: - Data Structures

/// An RGBA colour.
struct colour_t
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
