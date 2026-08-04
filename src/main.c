// OpenGL Libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Important Libraries
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Rendering
#include "shaderhandle.h"
#include "renderstate.h"
#include "render.h"

// MIDI libraries
#include "mididef.h"
#include "upload2gpu.h"

int main(void) {

    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // we are using 4.3 so that we can use SSBOs
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(500, 500, "RAMidi", NULL, NULL);
    if (window == NULL) { 
        fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // make so that every thing we do here, we focus on doing it in this window
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD.\n");
        return -1;
    }

    glfwSwapInterval(0); // turn vsync off

    glDisable(GL_DEPTH_TEST); // also turn off depth testing (this is for 3d!!! why is this enabled by default...)

    glfwSetFramebufferSizeCallback(window, beingresized); // using function in render.h

    // ------------------------------------------------------------ GPU STUFF ---------------------------------------------------------

    // Defining a SSBO to the GPU -----------------------------------------------------------------------------------------------------

    // creating a completely unnecessary vao, just do it
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint ssbo;
    glGenBuffers(1, &ssbo); // generate a buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo); // and start using it

    // ----------------------------------------------------------- RENDER STATE -------------------------------------------------------

    RenderState state;

    state.shaderProgram = shaderHandling();
    if (!state.shaderProgram) return -1; // shader err handle

    state.ssbo = ssbo;
    state.totalVisibleNotes = 1;

    state.stagingBuffer = NULL; // init pointer to nothing
    state.stagingLimit = 0;

    glfwSetWindowUserPointer(window, &state);

    // ---------------------------------------------------- FALLING NOTES MIDI VALUES -------------------------------------------------

    uint32_t tick = 0;

    // First create the pitch tracks

    PitchTrack testTracks[2];
    // no more order anymore! O(1) solves everything
    testTracks[0] = (PitchTrack){20, 0, 2};
    testTracks[1] = (PitchTrack){100, 2, 4};

    // our notes for each track

    // depending by how the shader is configured, the order will matter. for me it matters cuz that improves on performance lol
    Note visibleNotes[4];
    visibleNotes[0] = (Note){{0, 200}};
    visibleNotes[1] = (Note){{400, 100}};
    visibleNotes[2] = (Note){{10, 50}};
    visibleNotes[3] = (Note){{500, 200}};

    // ------------------------------------------------------ UPLOAD DATA TO THE GPU --------------------------------------------------

    uploadChunk64(&state, testTracks, 2, visibleNotes, 4);

    // ------------------------------------------------------------- MAIN LOOP --------------------------------------------------------

    while (!glfwWindowShouldClose(window)) {

        render(window, &state);
        
        glfwPollEvents();
    }

    glfwTerminate();

    free(state.stagingBuffer); // free ram to prevent leak

    return 0;
}