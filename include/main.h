#include "LeapC.h"

#ifndef main_h
#define main_h

#define TRACKER_POINTS_INDEX_FINGER index.distal.next_joint
#define TRACKER_POINTS_PALM palm.position

typedef int clickMode_t;

typedef struct screenCoordinates_t{
    int x;
    int y;

} screenCoordinates_t;

typedef struct screenCorners_t{
    int screenNum;
    screenCoordinates_t upperLeft;
    screenCoordinates_t upperRight;
    screenCoordinates_t bottomLeft;
    screenCoordinates_t bottomRight;

} screenCorners_t;

typedef struct leapCorners_t{
    LEAP_VECTOR upperLeft;
    LEAP_VECTOR upperRight;
    LEAP_VECTOR bottomLeft;
    LEAP_VECTOR bottomRight;
} leapCorners_t;

enum screenCorners_e{
    screenCorners_upperLeft,
    screenCorners_upperRight,
    screenCorners_bottomLeft,
    screenCorners_bottomRight
};

enum clickModes{
    clickModes_swing,
    clickModes_pinch
};

#endif