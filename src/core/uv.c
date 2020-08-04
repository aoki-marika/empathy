#include "uv.h"

// MARK: - Functions

struct uv_t uv(float u, float v)
{
    struct uv_t uv;
    uv.u = u;
    uv.v = v;

    return uv;
}

struct uv_t uv_zero()
{
    return uv(0, 0);
}
