#include "matrix.h"

// MARK: - Functions

struct matrix4_t matrix4(float e00, float e10, float e20, float e30,
                         float e01, float e11, float e21, float e31,
                         float e02, float e12, float e22, float e32,
                         float e03, float e13, float e23, float e33)
{
    // yes this looks messy, I know
    struct matrix4_t matrix;

    // row 0
    matrix.elements[0][0] = e00;
    matrix.elements[1][0] = e10;
    matrix.elements[2][0] = e20;
    matrix.elements[3][0] = e30;

    // row 1
    matrix.elements[0][1] = e01;
    matrix.elements[1][1] = e11;
    matrix.elements[2][1] = e21;
    matrix.elements[3][1] = e31;

    // row 2
    matrix.elements[0][2] = e02;
    matrix.elements[1][2] = e12;
    matrix.elements[2][2] = e22;
    matrix.elements[3][2] = e32;

    // row 3
    matrix.elements[0][3] = e03;
    matrix.elements[1][3] = e13;
    matrix.elements[2][3] = e23;
    matrix.elements[3][3] = e33;

    // return the constructed matrix
    return matrix;
}

struct matrix4_t matrix4_zero()
{
    return matrix4(
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    );
}

struct matrix4_t matrix4_identity()
{
    return matrix4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

struct matrix4_t matrix4_multiply(const struct matrix4_t left,
                                  const struct matrix4_t right)
{
    struct matrix4_t result = matrix4_zero();
    for (int column = 0; column < 4; column++)
    {
        for (int row = 0; row < 4; row++)
        {
            float sum = 0;
            for (int i = 0; i < 4; i++)
                sum += left.elements[i][row] * right.elements[column][i];

            result.elements[column][row] = sum;
        }
    }

    return result;
}

struct matrix4_t matrix4_orthographic(float l, float r, float t, float b, float n, float f)
{
    // parameter names are shortened as to not be visually cluttered
    return matrix4(
        2 / (r - l), 0,           0,           -(r + l) / (r - l),
        0,           2 / (t - b), 0,           -(t + b) / (t - b),
        0,           0,           2 / (f - n), -(f + n) / (f - n),
        0,           0,           0,           1
    );
}

struct matrix4_t matrix4_translation(struct vector3_t offset)
{
    return matrix4(
        1, 0, 0, offset.x,
        0, 1, 0, offset.y,
        0, 0, 1, offset.z,
        0, 0, 0, 1
    );
}
