#include "framebuffer.h"

// MARK: - Functions

void framebuffer_init(struct framebuffer_t *framebuffer,
                      unsigned int width,
                      unsigned int height)
{
    // create the framebuffer
    glGenFramebuffers(1, &framebuffer->id);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

    // create and bind the texture
    // framebuffers cant have an alpha component, and should always linear scale
    texture_init_empty(&framebuffer->texture,
                       width,
                       height,
                       TEXTURE_LINEAR,
                       TEXTURE_RGBU8);

    // GL_TEXTURE_2D is already bound properly from the texture init
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           framebuffer->texture.id,
                           0);

    // unbind the new framebuffer to ensure nothing is accidentally rendered to it
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_deinit(struct framebuffer_t *framebuffer)
{
    texture_deinit(&framebuffer->texture);
    glDeleteFramebuffers(1, &framebuffer->id);
}

void framebuffer_use(struct framebuffer_t *framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    glClear(GL_COLOR_BUFFER_BIT);
}
