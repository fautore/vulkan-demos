#include <cstdint>
#include <cstdlib>

#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>

#include "vulkan.cpp"

GLFWwindow* createWindow(uint32_t width, uint32_t height){
	glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        GLFWwindow* window = glfwCreateWindow(width, height, "MyHelloTriangle", nullptr, nullptr);
        glfwSetFramebufferSizeCallback(window, nullptr);
	return window;
}

struct app {
	uint32_t width;
	uint32_t height;
	GLFWwindow* window;
	vulkan vulkanInstance;
	
	static app create(uint32_t width, uint32_t height) {
		GLFWwindow* window = createWindow(width, height);
		vulkan vulkanInstance = vulkan::create(window);

		return app{
			.width = width,
			.height = height,
			.window = window,
			.vulkanInstance = vulkanInstance 
		};
	}
};

int main() {
	app a;
	try {
		a = app::create(800, 600);
		glfwSetWindowUserPointer(a.window, &a);
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
