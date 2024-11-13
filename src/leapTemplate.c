#include "LeapC.h"
#include "utils.h"
#include "leapTemplate.h"

int getFrame(LEAP_CONNECTION leapConnection,LEAP_CONNECTION_MESSAGE* frame){

    LEAP_CONNECTION_MESSAGE msg;
    int completedFlag = 0;
    eLeapRS rs;

    while(1){
        rs = LeapPollConnection(leapConnection,0,&msg);
        if(msg.type == eLeapEventType_Tracking){
            *frame = msg;
            completedFlag = 1;
        }

        if(rs == eLeapRS_Timeout) break;
    }

    return !completedFlag;
}

void handStack_append(leapHandStack_t* handStack, LEAP_HAND* hand){
    for(int i=HAND_STACK_MAX_LEN-1; i>0; i--){
        handStack->stack[i] = handStack->stack[i-1];
    }

    handStack->stack[0] = *hand;

    if(handStack->len < 10)
        handStack->len++;
}

int handStack_getInterpolation(leapHandStack_t* handStack, LEAP_HAND* hand, int interpolationLevel){
    if(interpolationLevel >= HAND_STACK_MAX_LEN) return 1;
    if(interpolationLevel >= handStack->len) return 1;
    LEAP_HAND* handA = handStack->stack;
    LEAP_HAND* handB = handStack->stack + interpolationLevel;

    // ------- BASIC    
    hand->id = handStack->stack[0].id;
    hand->flags = 0;
    hand->type = eLeapHandType_Right;
    hand->confidence = 1.0;
    hand->visible_time = handStack->stack[0].visible_time;
    
    // ------- FLOATS
    hand->pinch_distance = lerpFloat(handA->pinch_distance,handB->pinch_distance,0.5);
    hand->grab_angle     = lerpFloat(handA->grab_angle,handB->grab_angle,0.5);
    hand->pinch_strength = lerpFloat(handA->pinch_strength,handB->pinch_strength,0.5);
    hand->grab_strength  = lerpFloat(handA->grab_strength,handB->grab_strength,0.5);

    // ------- PALM
    hand->palm.position            = lerpLeapVector(handA->palm.position,handB->palm.position,0.5);
    hand->palm.stabilized_position = lerpLeapVector(handA->palm.stabilized_position,handB->palm.stabilized_position,0.5);
    hand->palm.velocity            = lerpLeapVector(handA->palm.velocity,handB->palm.velocity,0.5);
    hand->palm.normal              = lerpLeapVector(handA->palm.normal,handB->palm.normal,0.5);
    hand->palm.width               = lerpFloat(handA->palm.width,handB->palm.width,0.5);
    hand->palm.direction           = lerpLeapVector(handA->palm.direction,handB->palm.direction,0.5);
    hand->palm.orientation         = lerpLeapQuaternion(handA->palm.orientation,handB->palm.orientation,0.5);

    // -------- DIGITS
    lerpLeapDigit(&handA->thumb,&handB->thumb,&hand->thumb,0.5);
    lerpLeapDigit(&handA->index,&handB->index,&hand->index,0.5);
    lerpLeapDigit(&handA->middle,&handB->middle,&hand->middle,0.5);
    lerpLeapDigit(&handA->ring,&handB->ring,&hand->ring,0.5);
    lerpLeapDigit(&handA->pinky,&handB->pinky,&hand->pinky,0.5);
    lerpLeapBone(&handA->arm,&handB->arm,&hand->arm,0.5);

    return 0;
}