#include <stdint.h>

// struct to note
typedef struct {
    struct {
        uint32_t delStart: 17; // bits 
        uint32_t duration: 15; // bits
    } timing;
} __attribute__((packed)) Note; // make it packed so we DONT HAVE ANY UNUSED BYTES AT ALL! (this removes c's annoying padding)

// struct to store the keys that have the exact pitch
typedef struct {
    uint64_t key : 8;
    uint64_t start : 24; // start of where the notes are
    uint64_t end : 24; // end of the list
    uint64_t countOrExtra : 8; 

    /* 
    "why is the count of pitches here???" 
    sadly, sending 1 byte info to the gpu is not possible, so i decided to bring it to the first pitch track,
    the other pitch tracks can use this extra if there are a lot of keys. this extra value lets us go from 16 million notes (2^24) to 268 million (2^28)
    */
} __attribute__((packed)) PitchTrack;

/* do NOT make a struct that stores pitchtracks,
we have the goal of not storing everything in our ram,
if we do that to files with 9 GB were DONE!! you cant make a memory leak bro */