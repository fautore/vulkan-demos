CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

TARGET = vulkan-app

# directory paths
source_dir = src
binary_dir = bin
shaders_dir = shaders

# make executable
$(binary_dir)/$(TARGET): $(source_dir)/*.cpp $(source_dir)/*.hpp %.spv 
	g++ $(CFLAGS) -o $(binary_dir)/$(TARGET) $(source_dir)/*.cpp $(LDFLAGS)

# define shaders variables
glslc = /usr/bin/glslc
# create list of all spv files and set as dependency
vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))
# compile shaders
%.spv: $(shaders_dir)/*.vert $(shaders_dir)/*.frag	
	echo "compiling vert shaders..."
	echo $(vertSources)
	$(glslc) $(vertSources) -o$(vertObjFiles)
	$(glslc) $(fragSources) -o$(fragObjFiles)

.PHONY: test clean

test: $(binary_dir)/vulkan-app
	$(binary_dir)/vulkan-app

clean:
	rm -f a.out
	rm -f *.spv
