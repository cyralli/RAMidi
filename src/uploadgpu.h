// OpenGL Libs
#include <glad/glad.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mididef.h"
#include "renderstate.h"

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

    return;
}