#ifndef SHADERHANDLE_H
#define SHADERHANDLE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders/default_vert.h"
#include "shaders/default_frag.h"

#include <stdio.h>

static inline GLuint shaderHandling(void) {

    const char* vertSrcPoint = (const char*)src_shaders_default_vert;
    GLint vertLen = (GLint)src_shaders_default_vert_len;
    const char* fragSrcPoint = (const char*)src_shaders_default_frag;
    GLint fragLen = (GLint)src_shaders_default_frag_len;

    // creating space to shader, applying the source and compiling it.
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertSrcPoint, &vertLen);
    glCompileShader(vertexShader);

    // lets include error handling for shaders since glsl is pretty hard
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: Vertex shader compilation failed!\n%s", infoLog);
        return 0;
    }

    // do the same to the fragment shader
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSrcPoint, &fragLen);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: Fragment shader compilation failed!\n%s", infoLog);
        return 0; // ret false
    }

    // after making these shaders, we later make the program, that links the two shaders into a single program
    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram); // then after that we link it to the gpu

    // error handling to the program
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: Linking shaders to program failed!\n%s", infoLog);
        return 0; // ret false
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    // if all that worked, then we can return the shader program id
    return shaderProgram;
}

#endif