 #version 430 core

in vec2 uv;
out vec4 fragColor;

uniform float tick;

// Shader settings
const float maxNotes = 128.0;
const float noteWidth = 1.0 / maxNotes; // 1 divided by the max note
const float BORDER = 0.07 * noteWidth;

layout(std430, binding = 0) buffer ChunkData {

    uint trackLookup[32]; // 32 because glsl cant have values of 1 byte (128 / size of uint, which is 4 = 32)
    uint rawData[]; // then the resttttt

};

// --------------------------------- Main loop ---------------------------------


void main() {

    // background
    fragColor = vec4(0.1059, 0.1255, 0.1333, 1.0);

    // 1. we calculate our pixels key position
    uint currentPixelKey = uint(uv.x * maxNotes);
    if (currentPixelKey >= 128u) return; // at this point were just out of bounds bro

    // 2. O(1) is here!!!!!!!
    // unfortunately in glsl, when it comes to memory its TERRIBLE at doing that job
    // so we are being completely obligated to access 4 other lookups, which will be inside a uint
    // we can then, use bit masking

    uint targetIdx = (trackLookup[currentPixelKey / 4u] >> (currentPixelKey % 4) * 8) & 0xFF;

    if (targetIdx != 0xFF) { // if pixel is a active key continue. had to go with nesting LOL because its faster than returning

        // 3. get info from 1st track
        uint firstTrackUpper = rawData[1u];
        uint totalActivePitches = firstTrackUpper >> 24u;
        
        // the binary search really found a idx!

        uint stride = targetIdx * 2u;
        uint lower32 = rawData[stride]; // this contains the key, and the startIdx
        uint upper32 = rawData[stride + 1u];

        uint startIdx = lower32 >> 8u; // we remove the key by pushing 8 bits to right
        uint endIdx = upper32 & 0xFFFFFF; // for the endidx we simply mask the first 24 bits

        uint noteArrayOffset = totalActivePitches * 2u; // offset to where does the note arrays start (every pitchtracks is 2 uints)

        uint pixelTime = uint(uv.y * 1000.0); // also calculate the y position

        for (uint n = startIdx; n < endIdx; n++) {

            uint timing = rawData[noteArrayOffset + n];

            uint delStart = timing & 0x1FFFF; // mask 17 bits, since thats the size of our delStart in C
            uint duration = timing >> 17u; // just eliminate the rest now

            // if our notes are sorted by time then it gets faster by this if
            if (delStart > pixelTime) break;

            if (pixelTime > delStart && pixelTime < (delStart + duration)) {
                // THIS PIXEL IS INSIDE A NOTE!

                fragColor = vec4(0.1, 0.7, 1.0, 1.0);
                break;

            }
        }
    }
} 