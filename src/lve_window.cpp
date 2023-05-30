#pragma once

#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lve {
class LveWindow {
public:
  LveWindow(int w, int h, std::string name)
      : width{w}, height{h}, windowName{name} {
    initWindow();
  }

  ~LveWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  LveWindow(const LveWindow &) = delete;
  LveWindow &operator=(const LveWindow &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); }

  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface");
    }
  }

private:
  void initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window =
        glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  }

  const int width;
  const int height;
  std::string windowName;

  GLFWwindow *window;
};
} // namespace lve
