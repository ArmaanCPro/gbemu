#include "window.h"

#include <iostream>
#include <sstream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace glfw_callbacks
{
    static void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }
}

void APIENTRY glDebugOutput(GLenum source, GLenum, unsigned int id, GLenum severity,
                            GLsizei, const char* message, const void*)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return; // Ignore notification severity messages

    std::stringstream outputstream;
    outputstream << "---------------\n";
    outputstream << "Debug message (" << id << "): " << message << '\n';

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            outputstream << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            outputstream << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            outputstream << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            outputstream << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            outputstream << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            outputstream << "Source: Other";
            break;
        default:
            outputstream << "Source: Unknown";
    }
    std::cerr << outputstream.str() << std::endl;
}

window::window(int scr_width, int scr_height, std::string title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef GB_APPLE
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window_ = glfwCreateWindow(scr_width, scr_height, title.c_str(), nullptr, nullptr);
    if (window_ == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window_);
    glfwSetWindowUserPointer(window_, this); // you can access the window class through user pointer w/ window_
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // normal cursor for now
    glfwSetFramebufferSizeCallback(window_, glfw_callbacks::framebuffer_size_callback);

    // load glad opengl function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        exit(-1);
    }
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    if (GLAD_GL_VERSION_4_3)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
#endif
    glViewport(0, 0, scr_width, scr_height);
}

window::~window()
{
    glfwTerminate();
    window_ = nullptr;
}
