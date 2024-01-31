CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

shaders_sources_dir = shaders
shaders_binary_dir = shaders

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
bin/MyHelloTriangle: src/main.cpp $(VERT_SPIRV) $(FRAG_SPIRV)
	@g++ $(CFLAGS) -o bin/MyHelloTriangle src/main.cpp $(LDFLAGS)

.PHONY: test clean

test: bin/HelloTriangle
	bin/HelloTriangle

clean:
	rm -f a.out
	rm -f *.spv
