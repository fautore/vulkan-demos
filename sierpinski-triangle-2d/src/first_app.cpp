#include "lve_device.hpp"
#include "lve_model.cpp"
#include "lve_pipeline.cpp"
#include "lve_swap_chain.hpp"
#include "lve_window.cpp"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

const int NORMAL_MODE = 0;
const int SIERPINSKY_MODE = 1;
const int ANIMATION_MODE = 2;

namespace lve {
struct SimplePushConstantData {
  glm::vec2 offset;
  alignas(16) glm::vec3 color;
};

class FirstApp {
public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;
  static constexpr int mode = NORMAL_MODE;
  int sierpinskyDepth;

  FirstApp() {
    std::cout << "Welcome to hellotriangle !" << '\n';
    if (this->mode == 0) {
      std::cout << "Currently in normal mode" << '\n';
      loadModels();
    } else if (this->mode == 1) {
      std::cout << "currently in sierpinskyMode... please select triangle depth: ";
      std::string strdepth = "";
      std::cin >> strdepth;
      int depth{std::stoi(strdepth)};
      this->sierpinskyDepth = depth;
      std::cout << "Depth is set to: " << this->sierpinskyDepth << '\n';
      loadModels(this->sierpinskyDepth);
    }
    createPipelineLayout();
    recreateSwapChain();
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

  std::vector<LveModel::Vertex> makeSierpinski(std::vector<LveModel::Vertex> vertices, int depth) {
    std::vector<LveModel::Vertex> t1{
      vertices[0],
      getMidPoint(vertices[0], vertices[1]),
      getMidPoint(vertices[0], vertices[2])
    };
    std::vector<LveModel::Vertex> t2{
      getMidPoint(vertices[1], vertices[0]),
      vertices[1],
      getMidPoint(vertices[1], vertices[2])
    };
    std::vector<LveModel::Vertex> t3{
      getMidPoint(vertices[2], vertices[0]),
      getMidPoint(vertices[2], vertices[1]),
      vertices[2]
    };
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
    std::vector<LveModel::Vertex> vertices{{{0, -0.5f}, {1.0f, 0.0f, 0.0f}},
                                           {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                                           {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    if (this->mode == SIERPINSKY_MODE) {
      vertices = makeSierpinski(vertices, depth);
    }

    std::cout << "vertices: " << '\n';
    for (auto v : vertices) {
      printVertex(v);
    }
    std::cout << "total vertices: " << vertices.size() << '\n';

    lveModel = std::make_unique<LveModel>(lveDevice, vertices);
  }

  void createPipelineLayout() {

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr,
                               &pipelineLayout) != VK_SUCCESS) {
      throw new std::runtime_error("Failed to create pipeline layout");
    }
  }
  void createPipeline() {
    assert(lveSwapChain != nullptr &&
           "Cannot create pipeline before swap chain");
    assert(pipelineLayout != nullptr &&
           "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = lveSwapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    lvePipeline = std::make_unique<LvePipeline>(
      lveDevice,
      "bin/shaders/simple_shader.vert.spv",
      "bin/shaders/simple_shader.frag.spv",
      pipelineConfig
    );
  };

  void createCommandBuffers() {
    commandBuffers.resize(
        lveSwapChain
            ->imageCount()); // 2 or 3 depending on double or triple buffering
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = lveDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
    if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo,
                                 commandBuffers.data()) != VK_SUCCESS) {
      throw new std::runtime_error("failed to allocate command buffers");
    }
  }

  void freeCommandBuffers() {
    vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(),
                         static_cast<uint32_t>(commandBuffers.size()),
                         commandBuffers.data());
    commandBuffers.clear();
  }

  void recreateSwapChain() {
    auto extent = lveWindow.getExtent();
    while (extent.width == 0 || extent.height == 0) {
      extent = lveWindow.getExtent();
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(lveDevice.device());
    if (lveSwapChain == nullptr) {
      lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
    } else {
      lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent,
                                                    std::move(lveSwapChain));
      if (lveSwapChain->imageCount() != commandBuffers.size()) {
        freeCommandBuffers();
        createCommandBuffers();
      }
    }

    createPipeline();
  }

  void recordCommandBuffer(int imageIndex) {
    static int frame = 0;
    frame = (frame + 1) % 1000;

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) !=
        VK_SUCCESS) {
      throw std::runtime_error("command buffer failed to begin recording");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = lveSwapChain->getRenderPass();
    renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width =
        static_cast<float>(lveSwapChain->getSwapChainExtent().width);
    viewport.height =
        static_cast<float>(lveSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

    lvePipeline->bind(commandBuffers[imageIndex]);
    lveModel->bind(commandBuffers[imageIndex]);

    for (int j = 0; j < 4; j++) {
      SimplePushConstantData push{};
      push.offset = {-0.5f + frame * 0.002f, -0.4f + j * 0.25f};
      push.color = {0.0f, 0.0f, 0.2f + 0.2f * j};

      vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout,
                         VK_SHADER_STAGE_VERTEX_BIT |
                             VK_SHADER_STAGE_FRAGMENT_BIT,
                         0, sizeof(SimplePushConstantData), &push);
      lveModel->draw(commandBuffers[imageIndex]);
    }

    vkCmdEndRenderPass(commandBuffers[imageIndex]);
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer");
    }
  }

  void drawFrame() {
    uint32_t imageIndex;
    auto result = lveSwapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image");
    }

    recordCommandBuffer(imageIndex);
    result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex],
                                                &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
        lveWindow.wasWindowResized()) {
      lveWindow.resetWindowResizedFlag();
      recreateSwapChain();
      return;
    }
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image");
    }
  }

  // end private function declaration
  LveWindow lveWindow{WIDTH, HEIGHT, "Vulkaaannnnn!"};
  LveDevice lveDevice{lveWindow};
  std::unique_ptr<LveSwapChain> lveSwapChain;
  std::unique_ptr<LvePipeline> lvePipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<LveModel> lveModel;
};
} // namespace lve
