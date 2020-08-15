#include "matrix.h"

#include <math.h>

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

struct matrix4_t matrix4_orthographic(float l,
                                      float r,
                                      float t,
                                      float b,
                                      float n,
                                      float f)
{
    return matrix4(
        2 / (r - l), 0,           0,           -(r + l) / (r - l),
        0,           2 / (t - b), 0,           -(t + b) / (t - b),
        0,           0,           2 / (f - n), -(f + n) / (f - n),
        0,           0,           0,           1
    );
}

struct matrix4_t matrix4_translation(struct vector3_t offset)
{
    float x = offset.x, y = offset.y, z = offset.z;
    return matrix4(
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    );
}

struct matrix4_t matrix4_scaling(struct vector3_t scale)
{
    float x = scale.x, y = scale.y, z = scale.z;
    return matrix4(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    );
}

struct matrix4_t matrix4_shearing(struct vector3_t shear)
{
    float x = shear.x, y = shear.y, z = shear.z;
    return matrix4(
        1, x, x, 0,
        y, 1, y, 0,
        z, z, 1, 0,
        0, 0, 0, 1
    );
}

struct matrix4_t matrix4_rotation(struct vector3_t angles)
{
    // construct a rotation matrix for each axis, then multiply them to get the result
    // x
    float x_sin = sinf(angles.x);
    float x_cos = cosf(angles.x);
    struct matrix4_t x = matrix4(
             1,      0,      0,      0,
             0,  x_cos, -x_sin,      0,
             0,  x_sin,  x_cos,      0,
             0,      0,      0,      1
    );

    // y
    float y_sin = sinf(angles.y);
    float y_cos = cosf(angles.y);
    struct matrix4_t y = matrix4(
         y_cos,      0,  y_sin,      0,
             0,      1,      0,      0,
        -y_sin,      0,  y_cos,      0,
             0,      0,      0,      1
    );

    // z
    float z_sin = sinf(angles.z);
    float z_cos = cosf(angles.z);
    struct matrix4_t z = matrix4(
         z_cos, -z_sin,      0,      0,
         z_sin,  z_cos,      0,      0,
             0,      0,      1,      0,
             0,      0,      0,      1
    );

    // calculate and return the result
    struct matrix4_t matrix = matrix4_identity();
    matrix = matrix4_multiply(matrix, x);
    matrix = matrix4_multiply(matrix, y);
    matrix = matrix4_multiply(matrix, z);
    return matrix;
}
