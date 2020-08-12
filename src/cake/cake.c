#include "cake.h"

// MARK: - Functions

void cake_init(struct instance_t *instance,
               void *data)
{
    struct cake_t *cake = (struct cake_t *)data;
}

void cake_deinit(struct instance_t *instance,
                 void *data)
{
    struct cake_t *cake = (struct cake_t *)data;
}

void cake_render(struct instance_t *instance,
                 void *data,
                 struct framebuffer_t *framebuffer)
{
    struct cake_t *cake = (struct cake_t *)data;
}
