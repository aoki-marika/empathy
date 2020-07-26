#include "window.h"

// MARK: - Functions

void window_init(struct window_t *window,
                 unsigned int width,
                 unsigned int height,
                 const char *title,
                 bool is_resizable)
{
    // create the window
    glfwWindowHint(GLFW_RESIZABLE, (is_resizable) ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window->backing = glfwCreateWindow(width, height, title, NULL, NULL);

    // configure opengl
    window_set_current(window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create and bind the framebuffer and its texture
    glGenFramebuffers(1, &window->framebuffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, window->framebuffer_id);

    texture_init_empty(&window->framebuffer_texture,
                       width,
                       height,
                       TEXTURE_LINEAR,
                       TEXTURE_RGBU8);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           window->framebuffer_texture.id,
                           0);

    // unbind the framebuffer so nothing is accidentally rendered into it
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void window_deinit(struct window_t *window)
{
    texture_deinit(&window->framebuffer_texture);
    glDeleteFramebuffers(1, &window->framebuffer_id);
    glfwDestroyWindow(window->backing);
}

void window_set_current(struct window_t *window)
{
    glfwMakeContextCurrent(window->backing);
}

void window_set_background(struct window_t *window, const struct colour_t *colour)
{
    window_set_current(window);
    glClearColor(colour->r, colour->g, colour->b, colour->a);
}

bool window_is_closed(struct window_t *window)
{
    return glfwWindowShouldClose(window->backing) == GLFW_TRUE;
}

void window_begin_frame(struct window_t *window)
{
    window_set_current(window);
    glBindFramebuffer(GL_FRAMEBUFFER, window->framebuffer_id);
    glClear(GL_COLOR_BUFFER_BIT);
}

void window_begin_framebuffer_draw(struct window_t *window)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void window_end_frame(struct window_t *window)
{
    glfwSwapBuffers(window->backing);
}
