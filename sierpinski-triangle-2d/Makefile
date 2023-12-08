CFLAGS = -std=c++17 -I. -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

TARGET = vulkan-app

# shaders compiler
glslc = /usr/bin/glslc

# directory paths
source_dir = src
shaders_sources_dir = shaders
binary_dir = bin
shaders_binary_dir = bin/shaders

# Compile vertex shaders
$(shaders_binary_dir)/%.vert.spv: $(shaders_sources_dir)/%.vert
	@mkdir -p $(shaders_binary_dir)
	@glslc $< -o $@
# Compile fragment shaders
$(shaders_binary_dir)/%.frag.spv: $(shaders_sources_dir)/%.frag
	@mkdir -p $(shaders_binary_dir)
	@glslc $< -o $@

# compile shaders
VERT_SHADERS := $(wildcard $(shaders_sources_dir)/*.vert)
FRAG_SHADERS := $(wildcard $(shaders_sources_dir)/*.frag)
VERT_SPIRV := $(patsubst $(shaders_sources_dir)/%.vert, $(shaders_binary_dir)/%.vert.spv, $(VERT_SHADERS))
FRAG_SPIRV := $(patsubst $(shaders_sources_dir)/%.frag, $(shaders_binary_dir)/%.frag.spv, $(FRAG_SHADERS))

# make executable
$(binary_dir)/$(TARGET): $(source_dir)/*.cpp $(source_dir)/*.hpp $(VERT_SPIRV) $(FRAG_SPIRV)
	@g++ $(CFLAGS) -o $(binary_dir)/$(TARGET) $(source_dir)/*.cpp $(LDFLAGS)

.PHONY: test clean

test: $(binary_dir)/vulkan-app
	$(binary_dir)/vulkan-app

clean:
	rm -f a.out
	rm -f *.spv
