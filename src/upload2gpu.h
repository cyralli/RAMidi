#ifndef UPLOADGPU_H
#define UPLOADGPU_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mididef.h"
#include "renderstate.h"

static inline void uploadChunk64(RenderState *state, PitchTrack *tracks, uint32_t totalTracks, Note *notes, uint32_t totalNotes) {

    // first, create the lookup table for our pitchs, to reduce to O(1)
    uint8_t pitchLookup[128];

    memset(pitchLookup, 0xFF, sizeof(pitchLookup));

    for (uint8_t i = 0; i < totalTracks; ++i) {
        pitchLookup[tracks[i].key] = i;
    }
    
    // PASSING TOTAL TRACKS
    if (totalTracks > 0) {
        tracks[0].countOrExtra = totalTracks;
    }

    // find size of everything
    // finding size of pitchLookup is literally just a uint8_t * 128, or you know, 128 bytes!
    size_t trackSize = totalTracks * sizeof(PitchTrack);
    size_t noteSize = totalNotes * sizeof(Note);
    size_t totalBytes = 128 + trackSize + noteSize; // notice that 128 is the pitchLookup
    
    if (totalBytes > state->stagingLimit) {
        // then we allocate more ram (if thats possible)
        state->stagingLimit = totalBytes * 2; // add more in limit
        state->stagingBuffer = (uint8_t*)realloc(state->stagingBuffer, state->stagingLimit);

        printf("Reallocated more RAM (%zu bytes) in the MIDI loader.\n", state->stagingLimit);
    }

    // now after we have space to the buffer we can put our values to the gpu
    memcpy(state->stagingBuffer, pitchLookup, 128);
    memcpy(state->stagingBuffer + 128, tracks, trackSize); // copy our tracks to the stagingBuffer
    memcpy(state->stagingBuffer + 128 + trackSize, notes, noteSize); // same to notes now

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, state->ssbo); // use the ssbo

    glBufferData(GL_SHADER_STORAGE_BUFFER, totalBytes, state->stagingBuffer, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state->ssbo);
}

#endif