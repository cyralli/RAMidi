#version 430 core

in vec2 uv;
out vec4 fragColor;

uniform float time;

// Shader settings
const float maxNotes = 128.0;

const float noteWidth = 1.0 / maxNotes; // 1 divided by the max note
const float BORDER = 0.07 * noteWidth;

layout(std430, binding = 0) buffer ChunkData {
    uint rawData[];
};

// --------------------------------- Main loop ---------------------------------

void main() {

    // background
    fragColor = vec4(0.1059, 0.1255, 0.1333, 1.0);

    // 1. we calculate our pixels key position
    uint currentPixelKey = uint(uv.x * maxNotes);

    // 2. get info from first track

    uint firstTrackUpper = rawData[1u];
    uint totalActivePitches = firstTrackUpper >> 24u;

    if (totalActivePitches == 0u) return; // if there isnt any pitch active we just skip rendering notes.

    /* NOTE FOR PEOPLE WHO ARE READING THIS CODE:
    * Hey! as of right now, this fragment shader includes a whole searching algorithm
    * So why am i talking about it? The problem with this system is that: meanwhile its good for the RAM (which is exactly what this project is for) its also slow
    * The problem with it is that Binary searching is, in the Big-O notation, its average time is in O(log n)
    * That isn't really slow, but when you end up doing that for a big screen (mine is 1080p), you end up with a problem.

    * Instead its better to go with the O(1) method, where the track is immediately found.
    * However, doing this requires us to create a array of 128 bytes (or 256 bytes if the midi key range is bigger)
    * This isnt really a big deal, but yeah its good to know why am i doing this

    * Only this commit will have the binary search, it is not going to be incldued in the next commits.

    */

    // -------------------------------------------------- BINARY SEARCH ----------------------------------------------------

    // our pitch tracks are formed in arrays like this:
    // key 1, key 5, key 7, key 8
    // we did this so our ram doesnt waste space on empty keys

    // however, now we need to search for our desired key
    // linear is the most simple way of doing it, but its also slow
    // so, instead we can use binary search! https://en.wikipedia.org/wiki/Binary_search

    // however, calling to search in EVERY single pixel is a huge loss of performance
    // so instead we NERF it, we can ignore searching in pixels that their desired keys are out of the lowest and highest pitches

    uint lowestKey  = rawData[0u] & 0xFFu;
    uint highestKey = rawData[(totalActivePitches - 1u) * 2u] & 0xFFu;

    if (currentPixelKey < lowestKey || currentPixelKey > highestKey) {
        return;
    }

    // A in the article is rawData
    // n is the number of elements, so thats totalActivePitches
    // T is the target value, so its currentPixelKey

    int L = 0; // lowest element
    int R = int(totalActivePitches) - 1; // highest element
    int targetIdx = -1; // make the index that we want -1 to indicate if it fails or not, if its 0 or higher then we found a idx

    while (L <= R) {
        int m = (L + R) / 2; // middle
        uint stride = uint(m) * 2u;

        // so now we get our key value from this element
        uint key = rawData[stride] & 0xFFu; // mask first 8 bits

        if (key == currentPixelKey) {
            // we found our key.
            targetIdx = m;
            break; // stop loop
        } else if (key < currentPixelKey) {
            L = m + 1;
        } else {
            R = m - 1;
        }
    }
    if (targetIdx == -1) return; // if pixel doesnt have a active key, stop
    
    // the binary search really found a idx!
    uint stride = uint(targetIdx) * 2u;

    uint lower32 = rawData[stride]; // this contains the key, and the startIdx
    uint upper32 = rawData[stride + 1u];

    uint startIdx = lower32 >> 8u; // we remove the key by pushing 8 bytes to right
    uint endIdx = upper32 & 0xFFFFFF; // for the endidx we simply mask the first 24 bits
    
    uint noteArrayOffset = totalActivePitches * 2u; // offset to where does the note arrays start (every pitchtracks is 2 uints)

    uint pixelTime = uint(uv.y * 1000.0); // also calculate the y position

    for (uint n = startIdx; n < endIdx; n++) {
        uint timing = rawData[noteArrayOffset + n];

        uint delStart = timing & 0x1FFFF; // mask 17 bits, since thats the size of our delStart in C
        uint duration = timing >> 17u; // just eliminate the rest now

        if (pixelTime > delStart && pixelTime < (delStart + duration)) {
            fragColor = vec4(0.1, 0.7, 1.0, 1.0);
            break;
        }
    }
    
}