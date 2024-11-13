#include "xdo.h"
#include "main.h"
#ifndef utils
#define utils

typedef struct{
    float thumb, index, middle, ring, pinky;
} handAperture_t;

#pragma region // -------------------- PRINT FUNCTIONS ---------------------- //

void printLeapResult(int result);
void printLeapMessageType(eLeapEventType eventType);
void printLeapVector(LEAP_VECTOR vector, char* end);
void printMousePosition(xdo_t* xdo, char* end);
void printScreenCoordinates(screenCoordinates_t x);
void printScreenCorners(screenCorners_t corners);
void printLeapCorners(leapCorners_t corners);
void printHandAperture(handAperture_t *ap);

#pragma endregion

#pragma region // --------------------- LERP FUNCTIONS ---------------------- //

float lerpFloat(float A, float B, float T);
uint32_t lerpUint32(uint32_t A, uint32_t B, float T);
LEAP_VECTOR lerpLeapVector(LEAP_VECTOR A, LEAP_VECTOR B, float T);
LEAP_QUATERNION lerpLeapQuaternion(LEAP_QUATERNION A, LEAP_QUATERNION B, float T);
void lerpLeapBone(LEAP_BONE* A, LEAP_BONE* B, LEAP_BONE* OUT, float T);
void lerpLeapDigit(LEAP_DIGIT* A, LEAP_DIGIT* B, LEAP_DIGIT* OUT, float T);

#pragma endregion

#pragma region // ----------------------- VECTOR MATH ----------------------- //

float getVectorDotProduct(LEAP_VECTOR A, LEAP_VECTOR B);
float getDistanceBetweenVectors(LEAP_VECTOR A, LEAP_VECTOR B);
LEAP_VECTOR subLeapVector(LEAP_VECTOR A, LEAP_VECTOR B);

#pragma endregion

float getBoneLen(LEAP_BONE B);
float getDigitLen(LEAP_DIGIT D);
float getDigitAperture(LEAP_DIGIT D);
handAperture_t getHandAperture(LEAP_HAND *in);
int isHandOpen(LEAP_HAND *h, int ignoreIndex, handAperture_t* aperture);
int64_t getSystemTime();
void sleepFPS(int fps);
void printCalibrationMsg(int corner);

#endif