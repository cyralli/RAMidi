#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "renderstate.h"

static inline void render(GLFWwindow *window, RenderState* state) {
  
    float time = glfwGetTime();
    state->tick = time * 100;
 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->ssbo); // say were using this buffer right now.

    glUseProgram(state->shaderProgram);

    glUniform1ui(glGetUniformLocation(state->shaderProgram, "totalNotes"), state->totalVisibleNotes); // send the total of notes
    glUniform1f(glGetUniformLocation(state->shaderProgram, "tick"), state->tick);

    glDrawArrays(GL_TRIANGLES, 0, 3); // 3 verts somehow works LOL

    glfwSwapBuffers(window);
}

static inline void beingresized(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    #ifdef _WIN32 // when resizing a window in windows, the program freezes, so we also render when we are resizing
        RenderState* state = (RenderState*)glfwGetWindowUserPointer(window);

        if (state != NULL) {
            render(window, state);
        }
    #endif
}  