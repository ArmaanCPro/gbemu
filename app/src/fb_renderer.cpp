#include "fb_renderer.h"

#include <iostream>
#include <ostream>

fb_renderer::fb_renderer()
{
    glGenVertexArrays(1, &vao_id_);
    glGenBuffers(1, &vbo_id_);

    glBindVertexArray(vao_id_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);

    constexpr float scale_x = 1.0f, scale_y = 1.0f;

    // Quad vertices with positions and texture coordinates
    float vertices[] = {
        // positions  // texture coords
        -scale_x, scale_y,  0.0f, 1.0f, // top left
        -scale_x, -scale_y, 0.0f, 0.0f, // bottom left
        scale_x,  -scale_y, 1.0f, 0.0f, // bottom right

        -scale_x, scale_y,  0.0f, 1.0f, // top left
        scale_x,  -scale_y, 1.0f, 0.0f, // bottom right
        scale_x,  scale_y,  1.0f, 1.0f  // top right
    };
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fb_tex_id_);
    glBindTexture(GL_TEXTURE_2D, fb_tex_id_);
    // texture wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // texture filtering
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

void fb_renderer::render(const uint32_t* fb_data, uint32_t fb_width, uint32_t fb_height) const
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fb_tex_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)fb_width, (int)fb_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, fb_data);

    glBindVertexArray(vao_id_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
}

GLuint fb_renderer::create_shader_program()
{
    const GLchar* vert_shader_src = R"(
        #version 430 core
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
        #version 430 core
        out vec4 FragColor;
        in vec2 TexCoords;
        uniform sampler2D screenTexture;
        void main()
        {
            FragColor = texture(screenTexture, TexCoords);
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

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex);
    glAttachShader(shader_program, fragment);
    glLinkProgram(shader_program);
    // check for linking errors
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader_program, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return shader_program;
}
