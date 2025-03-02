#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

// renderer for a framebuffer
class fb_renderer
{
public:
    // this can be scaled, doesn't have to match the frame buffer dimensions
    fb_renderer(float window_width, float window_height);
    ~fb_renderer();

    // note that this doesn't take scaled parameters, it takes the exact frame_buffer dimensions
    void render(const uint32_t* fb_data, uint32_t fb_width, uint32_t fb_height);

private:
    // stores the texture id for the framebuffer
    GLuint fb_tex_id_ {0};
    GLuint vao_id_ {0}, vbo_id_ {0};
    GLuint fbo_id_ {0};
    GLuint shader_program_ {0};

    // helper method for creating a shader program
    GLuint create_shader_program();
};
