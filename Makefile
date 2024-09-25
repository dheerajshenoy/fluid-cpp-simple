install:
	emcc -o output/fluid-simple.html src/main.cpp -Os -Wall  -I. -I include -L. -L lib -l raylib -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB
