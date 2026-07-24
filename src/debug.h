// OpenGL Libs
#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include "renderstate.h"

static inline void debugRender(GLFWwindow *window, RenderState *state) {
    static double lastPrintTime;
    static uint32_t frameCount;

    double currentTime = glfwGetTime();

    state->deltaTime = currentTime - state->lastFrameTime;
    state->lastFrameTime = currentTime;
    ++frameCount;

    if (currentTime - lastPrintTime > 1) { // print one every 1 sec
        state->fps = (float)frameCount / (float)(currentTime - lastPrintTime);

        printf("\033[H\033[J"); // clear console (ansi)

        printf("Debug:\n");
        printf("FPS: %6.1f (%.5f ms/frame)\n", state->fps, state->deltaTime);
        printf("Allocated memory to Stage Buffer: %zu bytes\n", state->stagingLimit);

        frameCount = 0;
        lastPrintTime = currentTime;
    }
}