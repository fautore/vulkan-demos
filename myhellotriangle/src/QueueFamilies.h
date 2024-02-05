#include <cstdint>
#include <optional>
#include <vulkan/vulkan_core.h>
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete();
};
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice &device, VkSurfaceKHR &surface);
