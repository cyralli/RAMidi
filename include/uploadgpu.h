#ifndef UPLOADGPU_H
#define UPLOADGPU_H

#include <stdint.h>
#include "mididef.h"
#include "renderstate.h"

void uploadChunk64(RenderState *state, PitchTrack *tracks, uint32_t totalTracks, Note *notes, uint32_t totalNotes);

#endif