#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

// renderer for a framebuffer
class fb_renderer
{
public:
    fb_renderer();
    ~fb_renderer();

    void render(const uint32_t* fb_data, uint32_t fb_width, uint32_t fb_height);

private:
    // stores the texture id for the framebuffer
    GLuint fb_tex_id_ {0};
    GLuint vao_id_ {0}, vbo_id_ {0};
    GLuint shader_program_ {0};

    GLuint create_shader_program();
};
