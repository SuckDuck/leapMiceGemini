#include "LeapC.h"
#ifndef leaptemplate_def
#define leaptemplate_def

#define HAND_STACK_MAX_LEN 10
typedef struct leapHandStack_t{
    LEAP_HAND stack[HAND_STACK_MAX_LEN];
    int len;
} leapHandStack_t;

// READS THE ENTIRE MESSAGE QUEUE AND WRITES THE
// LAST TRACKING EVENT IN [frame] (IF THERE IS ANY).
// RETURNS 0 IF THERE IS A TRACKING EVENT
// RETURNS 1 IF THERE IS NO TRACKING EVENT 
int getFrame(LEAP_CONNECTION leapConnection,LEAP_CONNECTION_MESSAGE* frame);

// APPENDS A HAND TO POSITION 0 OF [handStack] AND
// SHIFT THE REST OF MSG'S TO THE RIGHT
void handStack_append(leapHandStack_t* handStack, LEAP_HAND* hand);

// GETS A HAND INTERPOLATION BETWEEN THE MOST RECENT HAND
// IN handStack AND A CUSTOM HAND IN [handStack]
// WORKS AS A KIND OF "DENOISING"
int handStack_getInterpolation(leapHandStack_t* handStack, LEAP_HAND* hand, int interpolationLevel);

#endif