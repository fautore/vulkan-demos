#include <cstdint>
#include <cstdlib>

#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>

#include "vulkan.cpp"

struct app {
	uint32_t width;
	uint32_t height;
	char* title;
	GLFWwindow* window;
	vulkan vulkanInstance;
	
	app(uint32_t width, uint32_t height): vulkanInstance() {
		this->width = width;
		this->height = height;
		// init window
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(width, height, "Myhellotriangle", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, nullptr);
	};
};

int main() {
	app a = app(800, 600);
	try {
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
        	return EXIT_FAILURE;
	}
    	try {
    	} catch (const std::exception& e) {
        	std::cerr << e.what() << std::endl;
        	return EXIT_FAILURE;
   	}
    	return EXIT_SUCCESS;
}
