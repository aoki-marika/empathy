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

    // initialize the background
    framebuffer->has_background = false;
}

void framebuffer_deinit(struct framebuffer_t *framebuffer)
{
    texture_deinit(&framebuffer->texture);
    glDeleteFramebuffers(1, &framebuffer->id);
}

void framebuffer_set_background(struct framebuffer_t *framebuffer, struct colour4_t colour)
{
    framebuffer->has_background = true;
    framebuffer->background_colour = colour;
}

void framebuffer_use(struct framebuffer_t *framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
    glViewport(0, 0, framebuffer->texture.width, framebuffer->texture.height);
    glClear(GL_COLOR_BUFFER_BIT);

    if (framebuffer->has_background)
        glClearBufferfv(GL_COLOR, 0, (float *)&framebuffer->background_colour);
}
