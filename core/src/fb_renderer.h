#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

// renderer for a framebuffer
class fb_renderer
{
public:
    fb_renderer(uint32_t fb_width, uint32_t fb_height);

    void render(const uint32_t* fb_data, uint32_t fb_width, uint32_t fb_height);

private:
    // stores the texture id for the framebuffer
    GLuint fb_tex_id_;

    GLuint vao_id_, vbo_id_;
    GLuint shader_program_;

    const GLchar* const vert_shader_ = R"(
        #version 430 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
            TexCoord = aTexCoord;
        }
    )";

    const GLchar* const frag_shader_ = R"(
        #version 430 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D screenTexture;
        void main()
        {
            FragColor = texture(screenTexture, TexCoord);
        }
    )";
};
