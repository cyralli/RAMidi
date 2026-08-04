#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <stdbool.h>

typedef struct {
    // these are values that are used when rendering

    // shader
    GLuint shaderProgram;
    GLuint ssbo;

    // values that the shader uses
    uint16_t totalVisibleNotes;
    uint32_t tick; // 4 bill, more than enough

    // memory
    uint8_t* stagingBuffer; // pointer to data
    size_t stagingLimit; // size

    // engine state
    bool isRunning;

    // still will be VERY important for later!
    double deltaTime;

} RenderState;

#endif