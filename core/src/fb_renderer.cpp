#include "fb_renderer.h"

#include <iostream>
#include <ostream>

fb_renderer::fb_renderer(float, float)
{
    glGenVertexArrays(1, &vao_id_);
    glGenBuffers(1, &vbo_id_);

    glBindVertexArray(vao_id_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);

    //glGenBuffers(1, &fbo_id_);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_id_);

    // calculate aspect ratios for custom scaling
    //float target_aspect = window_width / window_height;
    //float gb_aspect = 160.0f / 144.0f;

    float scale_x = 1.0f, scale_y = 1.0f;

    /*if (target_aspect > gb_aspect)
        scale_x = target_aspect / gb_aspect;
    else if (target_aspect < gb_aspect)
        scale_y = gb_aspect / target_aspect;*/

    // Quad vertices with positions and texture coordinates
    float vertices[] = {
        // positions  // texture coords
        -scale_x,  scale_y,  0.0f, 1.0f, // top left
        -scale_x, -scale_y,  0.0f, 0.0f, // bottom left
         scale_x, -scale_y,  1.0f, 0.0f, // bottom right

        -scale_x,  scale_y,  0.0f, 1.0f, // top left
         scale_x, -scale_y,  1.0f, 0.0f,  // bottom right
         scale_x,  scale_y,  1.0f, 1.0f // top right
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // texture coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glGenTextures(1, &fb_tex_id_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_tex_id_);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    shader_program_ = create_shader_program();
}

fb_renderer::~fb_renderer()
{
    glDeleteVertexArrays(1, &vao_id_);
    glDeleteBuffers(1, &vbo_id_);
    glDeleteTextures(1, &fb_tex_id_);
    glDeleteProgram(shader_program_);
}

void fb_renderer::render(const uint32_t*, uint32_t fb_width, uint32_t fb_height)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //glUseProgram(0);
    glUseProgram(shader_program_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_tex_id_);
    uint32_t test_data[160 * 144];
    for (int i = 0; i < 160 * 144; i++)
    {
        test_data[i] = 0xFF0000FF;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb_width, fb_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, test_data);
    glUniform1i(glGetUniformLocation(shader_program_, "u_Texture"), 0);

    glBindVertexArray(vao_id_);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_id_);
    //glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_tex_id_, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //glBlitFramebuffer(0, 0, fb_width, fb_height, 0, 0, fb_width, fb_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
}

GLuint fb_renderer::create_shader_program()
{
    const GLchar* vert_shader_src = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;
        out vec2 TexCoords;
        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
            TexCoords = aTexCoords;
        }
    )";

    const GLchar* frag_shader_src = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 TexCoords;
        //uniform sampler2D screenTexture;
        uniform sampler2D u_Texture;
        void main()
        {
            FragColor = texture(u_Texture, TexCoords);
        }
    )";

    GLint success;
    GLchar infoLog[512];

    // vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vert_shader_src, nullptr);
    glCompileShader(vertex);
    // check for shader compile errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &frag_shader_src, nullptr);
    glCompileShader(fragment);
    // check for shader compile errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    shader_program_ = glCreateProgram();
    glAttachShader(shader_program_, vertex);
    glAttachShader(shader_program_, fragment);
    glLinkProgram(shader_program_);
    // check for linking errors
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program_, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return shader_program_;
}
