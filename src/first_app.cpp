#include "lve_device.hpp"
#include "lve_model.cpp"
#include "lve_pipeline.cpp"
#include "lve_swap_chain.hpp"
#include "lve_window.cpp"
#include <GLFW/glfw3.h>

// std
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace lve {
class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  int sierpinskyDepth;

  FirstApp() {
    std::cout << "Welcome to hellotriangle !" << '\n';
    std::cout
        << "currently in sierpinskyMode... please select triangle depth: ";
    std::string strdepth = "";
    std::cin >> strdepth;
    int depth{std::stoi(strdepth)};
    this->sierpinskyDepth = depth;
    std::cout << "Depth is set to: " << this->sierpinskyDepth << '\n';
    loadModels(this->sierpinskyDepth);
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
  }
  ~FirstApp() {
    vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
  }

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void fpsCounter(bool enable, float fps) {
    if (!enable) {
      return;
    }
    std::cout << "FPS:" << fps << '\r';
  }

  void run() {
    while (!lveWindow.shouldClose()) {
      glfwPollEvents();

      auto start = std::chrono::high_resolution_clock::now();
      drawFrame();
      auto end = std::chrono::high_resolution_clock::now();
      double time_taken =
          std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
              .count();
      double seconds_taken = time_taken * 1e-9;
      double fps = 1 / seconds_taken;

      fpsCounter(false, fps);
    }
    vkDeviceWaitIdle(lveDevice.device());
  };

private:
  void printVertex(LveModel::Vertex v) {
    std::cout << "v: { " << v.position.x << ", " << v.position.y << " }"
              << '\n';
  }

  LveModel::Vertex getMidPoint(LveModel::Vertex v1, LveModel::Vertex v2) {
    LveModel::Vertex mid = {};
    mid.position.x = (v1.position.x + v2.position.x) / 2,
    mid.position.y = (v1.position.y + v2.position.y) / 2;
    return mid;
  };

  std::vector<LveModel::Vertex>
  makeSierpinski(std::vector<LveModel::Vertex> vertices, int depth) {
    std::vector<LveModel::Vertex> t1{vertices[0],
                                     getMidPoint(vertices[0], vertices[1]),
                                     getMidPoint(vertices[0], vertices[2])};
    std::vector<LveModel::Vertex> t2{getMidPoint(vertices[1], vertices[0]),
                                     vertices[1],
                                     getMidPoint(vertices[1], vertices[2])};
    std::vector<LveModel::Vertex> t3{getMidPoint(vertices[2], vertices[0]),
                                     getMidPoint(vertices[2], vertices[1]),
                                     vertices[2]};
    std::vector<LveModel::Vertex> newSierpinski{};
    if (depth < 1) {
      /*for (LveModel::Vertex vertex : t1) {
        newSierpinski.push_back(vertex);
      }
      for (LveModel::Vertex vertex : t2) {
        newSierpinski.push_back(vertex);
      }
      for (LveModel::Vertex vertex : t3) {
        newSierpinski.push_back(vertex);
      }*/
      return vertices;
    } else {
      depth = depth - 1;
      t1 = makeSierpinski(t1, depth);
      newSierpinski.insert(newSierpinski.end(), t1.begin(), t1.end());
      t2 = makeSierpinski(t2, depth);
      newSierpinski.insert(newSierpinski.end(), t2.begin(), t2.end());
      t3 = makeSierpinski(t3, depth);
      newSierpinski.insert(newSierpinski.end(), t3.begin(), t3.end());
      return newSierpinski;
    }
  }

  void loadModels(int depth = 0) {
    std::cout << "Depth iniziale: " << depth << '\n';
    std::vector<LveModel::Vertex> vertices{
        {{0, -1.0f}}, {{-1.0f, 1.0f}}, {{1.0f, 1.0f}}};

    vertices = makeSierpinski(vertices, depth);

    std::cout << "vertices: " << '\n';
    for (auto v : vertices) {
      printVertex(v);
    }
    std::cout << "total vertices: " << vertices.size() << '\n';

    lveModel = std::make_unique<LveModel>(lveDevice, vertices);
  }

  void createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr,
                               &pipelineLayout) != VK_SUCCESS) {
      throw new std::runtime_error("Failed to create pipeline layout");
    }
  }
  void createPipeline() {
    auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(
        lveSwapChain.width(), lveSwapChain.height());
    pipelineConfig.renderPass = lveSwapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    lvePipeline = std::make_unique<LvePipeline>(
        lveDevice, "shaders/simple_shader.vert.spv",
        "shaders/simple_shader.frag.spv", pipelineConfig);
  };
  void createCommandBuffers() {
    commandBuffers.resize(
        lveSwapChain
            .imageCount()); // 2 or 3 depending on double or triple buffering
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = lveDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo,
                                 commandBuffers.data()) != VK_SUCCESS) {
      throw new std::runtime_error("failed to allocate command buffers");
    }

    for (int i = 0; i < commandBuffers.size(); i++) {
      VkCommandBufferBeginInfo beginInfo{};
      beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

      if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("command buffer failed to begin recording");
      }

      VkRenderPassBeginInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = lveSwapChain.getRenderPass();
      renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

      std::array<VkClearValue, 2> clearValues{};
      clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
      clearValues[1].depthStencil = {1.0f, 0};
      renderPassInfo.clearValueCount =
          static_cast<uint32_t>(clearValues.size());
      renderPassInfo.pClearValues = clearValues.data();

      vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                           VK_SUBPASS_CONTENTS_INLINE);

      lvePipeline->bind(commandBuffers[i]);
      lveModel->bind(commandBuffers[i]);
      lveModel->draw(commandBuffers[i]);

      vkCmdEndRenderPass(commandBuffers[i]);
      if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer");
      }
    }
  }
  void drawFrame() {
    uint32_t imageIndex;
    auto result = lveSwapChain.acquireNextImage(&imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image");
    }

    result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex],
                                               &imageIndex);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image");
    }
  }
  LveWindow lveWindow{WIDTH, HEIGHT, "Vulkaaannnnn!"};
  LveDevice lveDevice{lveWindow};
  LveSwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<LveModel> lveModel;
};
} // namespace lve
