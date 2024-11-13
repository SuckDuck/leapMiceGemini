#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "LeapC.h"
#include "xdo.h"
#include "main.h"
#include "utils.h"

#pragma region // -------------------- PRINT FUNCTIONS ---------------------- //

void printLeapResult(int result){
    switch(result){
        case eLeapRS_BufferSizeOverflow:        printf("%s\n","BufferSizeOverflow");        break;
        case eLeapRS_CannotOpenDevice:          printf("%s\n","CannotOpenDevice");          break;
        case eLeapRS_ConcurrentPoll:            printf("%s\n","ConcurrentPoll");            break;
        case eLeapRS_HandshakeIncomplete:       printf("%s\n","HandshakeIncomplete");       break;
        case eLeapRS_InsufficientBuffer:        printf("%s\n","InsufficientBuffer");        break;
        case eLeapRS_InsufficientResources:     printf("%s\n","InsufficientResources");     break;
        case eLeapRS_InvalidArgument:           printf("%s\n","InvalidArgument");           break;
        case eLeapRS_InvalidClientID:           printf("%s\n","InvalidClientID");           break;
        case eLeapRS_NotAvailable:              printf("%s\n","NotAvailable");              break;
        case eLeapRS_NotConnected:              printf("%s\n","NotConnected");              break;
        case eLeapRS_NotStreaming:              printf("%s\n","NotStreaming");              break;
        case eLeapRS_ProtocolError:             printf("%s\n","ProtocolError");             break;
        case eLeapRS_RoutineIsNotSeer:          printf("%s\n","RoutineIsNotSeer");          break;
        case eLeapRS_Timeout:                   printf("%s\n","Timeout");                   break;
        case eLeapRS_TimestampTooEarly:         printf("%s\n","TimestampTooEarly");         break;
        case eLeapRS_UnexpectedClosed:          printf("%s\n","UnexpectedClosed");          break;
        case eLeapRS_UnknownError:              printf("%s\n","UnknownError");              break;
        case eLeapRS_UnknownImageFrameRequest:  printf("%s\n","UnknownImageFrameRequest");  break;
        case eLeapRS_UnknownTrackingFrameID:    printf("%s\n","UnknownTrackingFrameID");    break;
        case eLeapRS_Unsupported:               printf("%s\n","Unsupported");               break;
        default:                                printf("%s\n", "OK");                       break;
    }
}

void printLeapMessageType(eLeapEventType eventType){
    switch (eventType){
    case eLeapEventType_ConfigChange:        printf("%s\n","ConfigChange");        break;
    case eLeapEventType_ConfigResponse:      printf("%s\n","ConfigResponse");      break;
    case eLeapEventType_Connection:          printf("%s\n","Connection");          break;
    case eLeapEventType_ConnectionLost:      printf("%s\n","ConnectionLost");      break;
    case eLeapEventType_Device:              printf("%s\n","Device");              break;
    case eLeapEventType_DeviceFailure:       printf("%s\n","DeviceFailure");       break;
    case eLeapEventType_DeviceLost:          printf("%s\n","DeviceLost");          break;
    case eLeapEventType_DeviceStatusChange:  printf("%s\n","DeviceStatusChange");  break;
    case eLeapEventType_DroppedFrame:        printf("%s\n","DroppedFrame");        break;
    case eLeapEventType_Eyes:                printf("%s\n","Eyes");                break;
    case eLeapEventType_HeadPose:            printf("%s\n","HeadPose");            break;
    case eLeapEventType_Image:               printf("%s\n","Image");               break;
    case eLeapEventType_ImageComplete:       printf("%s\n","ImageComplete");       break;
    case eLeapEventType_ImageRequestError:   printf("%s\n","ImageRequestError");   break;
    case eLeapEventType_IMU:                 printf("%s\n","IMU");                 break;
    case eLeapEventType_LogEvent:            printf("%s\n","LogEvent");            break;
    case eLeapEventType_LogEvents:           printf("%s\n","LogEvents");           break;
    case eLeapEventType_NewDeviceTransform:  printf("%s\n","NewDeviceTransform");  break;
    case eLeapEventType_None:                printf("%s\n","None");                break;
    case eLeapEventType_PointMappingChange:  printf("%s\n","PointMappingChange");  break;
    case eLeapEventType_Policy:              printf("%s\n","Policy");              break;
    case eLeapEventType_Tracking:            printf("%s\n","Tracking");            break;
    case eLeapEventType_TrackingMode:        printf("%s\n","TrackingMode");        break;
    }
}

void printLeapVector(LEAP_VECTOR vector, char* end){
    printf("VECTOR: %f %f %f%s",vector.x,vector.y,vector.z,end == NULL ? "\n" : end);
}

void printMousePosition(xdo_t* xdo, char* end){
    int x, y, s;
    xdo_get_mouse_location(xdo,&x,&y,&s);
    printf("X:%i Y:%i SCREEN:%i%s",x,y,s,end == NULL ? "\n" : end);
}

void printScreenCoordinates(screenCoordinates_t x){
    printf("X:%i Y:%i\n",x.x, x.y);
}

void printScreenCorners(screenCorners_t corners){
    printf("Screen numer:%i\n",corners.screenNum);
    printf("Upper Left   --> X:%i Y:%i\n",corners.upperLeft.x,corners.upperLeft.y);
    printf("Upper Right  --> X:%i Y:%i\n",corners.upperRight.x,corners.upperRight.y);
    printf("Bottom Left  --> X:%i Y:%i\n",corners.bottomLeft.x,corners.bottomLeft.y);
    printf("Bottom Right --> X:%i Y:%i\n\n",corners.bottomRight.x,corners.bottomRight.y);

}

void printLeapCorners(leapCorners_t corners){
    printf("Upper Left   --> X:%f Y:%f Z:%f\n", corners.upperLeft.x,corners.upperLeft.y);
    printf("Upper Right  --> X:%f Y:%f Z:%f\n", corners.upperRight.x,corners.upperRight.y);
    printf("Bottom Left  --> X:%f Y:%f Z:%f\n",corners.bottomLeft.x,corners.bottomLeft.y);
    printf("Bottom Right --> X:%f Y:%f Z:%f\n\n",corners.bottomRight.x,corners.bottomRight.y);
}

void printHandAperture(handAperture_t *ap){
    printf("INDEX:%f MIDDLE:%f RING:%f PINKY:%f THUMB:%f\n",
            ap->index,ap->middle,ap->ring,ap->pinky,ap->thumb);
}

#pragma endregion

#pragma region // --------------------- LERP FUNCTIONS ---------------------- //

float lerpFloat(float A, float B, float T){
    return A+(B-A)*T;
}

uint32_t lerpUint32(uint32_t A, uint32_t B, float T){
    return (uint32_t)A+(B-A)*T;
}

LEAP_VECTOR lerpLeapVector(LEAP_VECTOR A, LEAP_VECTOR B, float T){
    LEAP_VECTOR returnVector;
    returnVector.x = lerpFloat(A.x,B.x,T);
    returnVector.y = lerpFloat(A.y,B.y,T);
    returnVector.z = lerpFloat(A.z,B.z,T);
    return returnVector;
}

LEAP_QUATERNION lerpLeapQuaternion(LEAP_QUATERNION A, LEAP_QUATERNION B, float T){
    LEAP_QUATERNION retrunQuaternion;
    retrunQuaternion.w = lerpFloat(A.w,B.w,T);
    retrunQuaternion.x = lerpFloat(A.x,B.x,T);
    retrunQuaternion.y = lerpFloat(A.y,B.y,T);
    retrunQuaternion.z = lerpFloat(A.z,B.z,T);
    return retrunQuaternion;
}

void lerpLeapBone(LEAP_BONE* A, LEAP_BONE* B, LEAP_BONE* OUT, float T){
    OUT->prev_joint = lerpLeapVector(A->prev_joint,B->prev_joint,T);
    OUT->next_joint = lerpLeapVector(A->next_joint,B->next_joint,T);
    OUT->width = lerpFloat(A->width,B->width,T);
    OUT->rotation = lerpLeapQuaternion(A->rotation,B->rotation,T);

}

void lerpLeapDigit(LEAP_DIGIT* A, LEAP_DIGIT* B, LEAP_DIGIT* OUT, float T){
    OUT->finger_id = A->finger_id;
    lerpLeapBone(&A->metacarpal,&B->metacarpal,&OUT->metacarpal,T);
    lerpLeapBone(&A->proximal,&B->proximal,&OUT->proximal,T);
    lerpLeapBone(&A->intermediate,&B->intermediate,&OUT->intermediate,T);
    lerpLeapBone(&A->distal,&B->distal,&OUT->distal,T);
    OUT->is_extended = lerpUint32(A->is_extended,B->is_extended,T);
}

#pragma endregion

#pragma region // ----------------------- VECTOR MATH ----------------------- //

float getVectorDotProduct(LEAP_VECTOR A, LEAP_VECTOR B){
    return (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
}

float getDistanceBetweenVectors(LEAP_VECTOR A, LEAP_VECTOR B){
    return (float) sqrt( pow((double)(A.x - B.x),2) + pow((double)(A.y - B.y),2) + pow((double)(A.z - B.z),2) ); 
}

LEAP_VECTOR subLeapVector(LEAP_VECTOR A, LEAP_VECTOR B){
    LEAP_VECTOR rs;
    rs.x = A.x - B.x;
    rs.y = A.y - B.y;
    rs.z = A.z - B.z;
    return rs;
}

#pragma endregion

float getBoneLen(LEAP_BONE B){
    return getDistanceBetweenVectors(B.prev_joint,B.next_joint);
}

float getDigitLen(LEAP_DIGIT D){
    return getBoneLen(D.distal) + getBoneLen(D.intermediate) + getBoneLen(D.proximal) + getBoneLen(D.metacarpal);
}

float getDigitAperture(LEAP_DIGIT D){
    float len = getDigitLen(D);
    float aperture = getDistanceBetweenVectors(D.distal.next_joint,D.metacarpal.prev_joint);
    return (aperture/len)*100;
}

handAperture_t getHandAperture(LEAP_HAND *in){
    handAperture_t aperture;
    aperture.index = getDigitAperture(in->index);
    aperture.middle = getDigitAperture(in->middle);
    aperture.ring = getDigitAperture(in->ring);
    aperture.pinky = getDigitAperture(in->pinky);
    aperture.thumb = getDigitAperture(in->thumb);
    return aperture;
}

int isHandOpen(LEAP_HAND *h, int ignoreIndex, handAperture_t* aperture){
    *aperture = getHandAperture(h);
    if(aperture->index < 93 && ignoreIndex == 0) return 1;
    if(aperture->middle < 93) return 1;
    if(aperture->ring < 93) return 1;
    if(aperture->pinky < 93) return 1;
    return 0;
}

int64_t getSystemTime(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)(tv.tv_sec) * 1000000 + tv.tv_usec;
}

void sleepFPS(int fps){
    int t = (int)((1.0/(float) fps) * 1000000);
    usleep(t);
}

void printCalibrationMsg(int corner){
    char* cornerName;
    printf("Place your cursor and index finger at the ");
    switch(corner){
        case 0:  printf("top left ");      break;
        case 1:  printf("top right ");     break;
        case 2:  printf("bottom left ");   break;
        case 3:  printf("bottom right ");  break;
    }

    printf("corner of the screen and hit Ctrl....");
    fflush(stdout);
}