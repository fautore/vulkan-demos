#include "lve_device.hpp"
#include "lve_pipeline.cpp"
#include "lve_window.cpp"
#include <GLFW/glfw3.h>

namespace lve {
class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  void run() {
    while (!lveWindow.shouldClose()) {
      glfwPollEvents();
    }
  };

private:
  LveWindow lveWindow{WIDTH, HEIGHT, "Vulkaaannnnn!"};
  LveDevice lveDevice{lveWindow};
  LvePipeline lvePipeline{
      lveDevice, "./shaders/simple_shader.vert.spv",
      "./shaders/simple_shader.frag.spv",
      LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
};
} // namespace lve
