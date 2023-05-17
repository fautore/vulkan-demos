CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vulkan-app: *.cpp *.hpp
	g++ $(CFLAGS) -o vulkan-app *.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkan-app
	./vulkan-app

clean:
	rm -f vulkan-app
