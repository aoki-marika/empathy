#include "vector.h"

// MARK: - Functions

struct vector3_t vector3(float x, float y, float z)
{
    struct vector3_t vector;
    vector.x = x;
    vector.y = y;
    vector.z = z;

    return vector;
}

struct vector3_t vector3_zero()
{
    return vector3(0, 0, 0);
}

