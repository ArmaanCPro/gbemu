#pragma once

#include <GLFW/glfw3.h>
#include <string>

// note this isn't in gb namespace. it doesn't depend on the game boy.
class window
{
public:
    window(int scr_width, int scr_height, std::string title);
    ~window();
    window(const window&) = delete;
    window& operator=(const window&) = delete;

    [[nodiscard]] bool should_close() const { return glfwWindowShouldClose(window_); }
    void swap_buffers() const { glfwSwapBuffers(window_); }
    void poll_events() { glfwPollEvents(); }
private:
    GLFWwindow* window_ = nullptr;
};
