CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

hellotriangle: main.cpp
	g++ $(CFLAGS) -o hellotriangle main.cpp $(LDFLAGS)

.PHONY: test clean

test: hellotriangle
	./hellotriangle

clean:
	rm -f hellotriangle
