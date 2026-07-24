// OpenGL Libs
#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

// Important Libraries
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// GL Custom Libs (making this readable)
#include "shaderhandle.h"
#include "debug.h"
#include "renderstate.h"

// MIDI libraries
#include "mididef.h"


void render(GLFWwindow *window, RenderState* state) {
    // some values that are needed instantly
    float time = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->ssbo); // say were using this buffer right now.

    glUseProgram(state->shaderProgram); // say we are using our shader program
    glUniform1ui(glGetUniformLocation(state->shaderProgram, "totalNotes"), state->totalVisibleNotes); // send the total of notes

    glUniform1f(glGetUniformLocation(state->shaderProgram, "time"), time); // time is also important!

    // draw 6 vertices to fill our screen!
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window); // get the next frame from the framebuffer the gpu prepares.
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    RenderState* state = (RenderState*)glfwGetWindowUserPointer(window);

    if (state != NULL) {
        render(window, state);
    }
}  

// Uploading MIDI data to the GPU

void uploadChunk64(RenderState *state, PitchTrack *tracks, uint32_t totalTracks, Note *notes, uint32_t totalNotes) {
    // pass the totalTracks to the extra of the first track
    if (totalTracks > 0) {
        tracks[0].countOrExtra = totalTracks;
    }

    size_t trackSize = totalTracks * sizeof(PitchTrack);
    size_t noteSize = totalNotes * sizeof(Note);
    size_t totalBytes = trackSize + noteSize;
    
    if (totalBytes > state->stagingLimit) {
        // then we allocate more ram (if thats possible)
        state->stagingLimit = totalBytes * 2; // add more in limit
        state->stagingBuffer = (uint8_t*)realloc(state->stagingBuffer, state->stagingLimit);

        printf("Reallocated more RAM (%zu bytes) in the MIDI loader.", state->stagingLimit);
    }

    // now after we have space to the buffer we can put our values to the gpu
    memcpy(state->stagingBuffer, tracks, trackSize); // copy our tracks to the stagingBuffer
    memcpy(state->stagingBuffer + trackSize, notes, noteSize); // same to notes now

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->ssbo); // use the ssbo

    glBufferData(GL_SHADER_STORAGE_BUFFER, totalBytes, state->stagingBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->ssbo);
}

int main(void) {

    if (!glfwInit()) return -1; // if glfw didnt start then we just return -1 (since it already uses glfwTerminate before returning)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // we are using 4.3 so that we can use SSBOs
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(500, 500, "Random Access MIDI", NULL, NULL);
    if (window == NULL) { // if window failed
        fprintf(stderr, "Failed to create GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // make so that every thing we do we focus on doing it in this window

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // load glad 
        fprintf(stderr, "Failed to initialize GLAD.\n");
        return -1;
    }

    glfwSwapInterval(0); // turn vsync off

    glDisable(GL_DEPTH_TEST); // also turn off depth testing (this is for 3d!!! why is this here)

    glViewport(0, 0, 500, 500);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // when the user changes the window size then change the gl viewport

    // ------------------------------------------------------------ GPU STUFF ---------------------------------------------------------

    // Defining a SSBO to the GPU -----------------------------------------------------------

    // lets create a VAO first, the gpu still needs it.
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); 

    GLuint ssbo;
    glGenBuffers(1, &ssbo); // generate a buffer
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo); // and start using it

    // ----------------------------------------------------------- RENDER STATE -------------------------------------------------------

    RenderState state;

    // shader
    state.shaderProgram = shaderHandling();
    if (!state.shaderProgram) return -1;

    state.ssbo = ssbo;
    state.totalVisibleNotes = 1;

    state.stagingBuffer = NULL;
    state.stagingLimit = 0;

    glfwSetWindowUserPointer(window, &state);

    // ---------------------------------------------------- FALLING NOTES MIDI VALUES -------------------------------------------------

    uint32_t tick = 0;

    // First create the pitch tracks

    PitchTrack testTracks[2];
    // ALWAYS MAKE THIS IN ORDER. seriously, PLEASE!
    testTracks[0] = (PitchTrack){20, 0, 2};
    testTracks[1] = (PitchTrack){30, 2, 4};

    // Then our notes for each track
    Note visibleNotes[4];
    visibleNotes[0] = (Note){{0, 200}}; 
    visibleNotes[1] = (Note){{400, 100}};
    visibleNotes[2] = (Note){{10, 50}}; 
    visibleNotes[3] = (Note){{500, 200}};

    // ------------------------------------------------------ UPLOAD DATA TO THE GPU --------------------------------------------------

    uploadChunk64(&state, testTracks, 2, visibleNotes, 4);

    // ------------------------------------------------------------- MAIN LOOP -------------------------------------------------------

    while (!glfwWindowShouldClose(window)) {
        // debug info
        debugRender(window, &state);

        render(window, &state);
        
        glfwPollEvents(); // check if any events are active (keyboard, mouse)
    }

    glfwTerminate();

    free(state.stagingBuffer); // free ram to prevent leak

    return 0;
}