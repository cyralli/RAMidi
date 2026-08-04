#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderstate.h"

void render(GLFWwindow *window, RenderState* state);

// lets put this here because i just dont feel like putting this junk in my rendering code, render.c IS focusing in rendering not resizing
static inline void beingresized(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    #ifdef _WIN32 // when resizing a window in windows, the program freezes, so we also render when we are resizing
        RenderState* state = (RenderState*)glfwGetWindowUserPointer(window);

        if (state != NULL) {
            render(window, state);
        }
    #endif
}  