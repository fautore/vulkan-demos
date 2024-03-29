#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "QueueFamilies.h"
#include "physicalDevice.h"

// device extensions to require
const std::vector<const char *> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(
		device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(
		device, nullptr, &extensionCount, availableExtensions.data());
	std::set<std::string> requiredExtensions(
		deviceExtensions.begin(), deviceExtensions.end());
	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR &surface) {
	QueueFamilyIndices indices = findQueueFamilies(device, surface);
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = true;
	// if (extensionsSupported) {
	// SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
	// swapChainAdequate = !swapChainSupport.formats.empty() &&
	// !swapChainSupport.presentModes.empty();
	// }
	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
	return indices.isComplete() && extensionsSupported && swapChainAdequate
		   && supportedFeatures.samplerAnisotropy;
}

VkPhysicalDevice selectPhisicalDevice(VkInstance &instance,
	VkSurfaceKHR &surface) {
	uint32_t devicesCount = 0;
	vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
	if (devicesCount == 0) {
		throw std::runtime_error("failed to find GPUs that support Vulkan!");
	}
	std::vector<VkPhysicalDevice> devices(devicesCount);
	vkEnumeratePhysicalDevices(instance, &devicesCount, devices.data());
	for (const auto &device : devices) {
		if (isDeviceSuitable(device, surface)) {
			return device;
		}
	}
	throw std::runtime_error("failed to find a suitable GPU!");
}
