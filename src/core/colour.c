#include "colour.h"

// MARK: - Functions

struct colour4_t colour4(float r, float g, float b, float a)
{
    struct colour4_t colour;
    colour.r = r;
    colour.g = g;
    colour.b = b;
    colour.a = a;

    return colour;
}
