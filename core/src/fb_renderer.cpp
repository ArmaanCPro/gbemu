#include "fb_renderer.h"

#include <iostream>
#include <ostream>
#include <vector>

fb_renderer::fb_renderer(uint32_t fb_width, uint32_t fb_height)
{
    // Define vertices for a quad that fills the entire screen
    const float quadVertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f,   // top left
         1.0f,  1.0f,  1.0f, 1.0f,   // top right
         1.0f, -1.0f,  1.0f, 0.0f,   // bottom right
        -1.0f,  1.0f,  0.0f, 1.0f,   // top left
         1.0f, -1.0f,  1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f,  0.0f, 0.0f    // bottom left
    };

    // Create and bind VAO first
    glGenVertexArrays(1, &vao_id_);
    glBindVertexArray(vao_id_);

    // Create and setup VBO
    glGenBuffers(1, &vbo_id_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Print the first vertex position for verification
    std::cout << "First vertex position: " << quadVertices[0] << ", " << quadVertices[1] << std::endl;

    glGenTextures(1, &fb_tex_id_);
    glBindTexture(GL_TEXTURE_2D, fb_tex_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Initialize with empty data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb_width, fb_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vert_shader_, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader compilation
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    }

    // Create fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &frag_shader_, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    }

    // Create shader program
    shader_program_ = glCreateProgram();
    glAttachShader(shader_program_, vertexShader);
    glAttachShader(shader_program_, fragmentShader);
    glLinkProgram(shader_program_);

    // Check program linking
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program_, 512, NULL, infoLog);
        std::cout << "Shader program linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void check_gl_error(const char* operation) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error after " << operation << ": 0x" << std::hex << err << std::dec << std::endl;
    }
}

void fb_renderer::render(const uint32_t* fb_data, uint32_t fb_width, uint32_t fb_height)
{
    // Update texture with new framebuffer data
    glBindTexture(GL_TEXTURE_2D, fb_tex_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb_width, fb_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, fb_data);

    // Clear and draw
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program_);
    glBindVertexArray(vao_id_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
