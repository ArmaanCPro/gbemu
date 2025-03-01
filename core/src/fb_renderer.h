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

    const GLchar* const vert_shader_ =
        "#version 330 core"
        "layout (location = 0) in vec2 aPos;"
        "layout (location = 1) in vec2 aTexCoords;"
        "out vec2 TexCoords;"
        "void main()"
        "{"
        "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);"
        "   TexCoords = aTexCoords;"
        "}";

    const GLchar* const frag_shader_ =
        "#version 330 core"
        "layout (location = 0) in vec2 aPos;"
        "in vec2 TexCoords;"
        "out vec4 FragColor;"
        "uniform sampler2D screenTexture;"
        "void main()"
        "{"
        "   FragColor = texture(screenTexture, TexCoords);"
        "}";
};
