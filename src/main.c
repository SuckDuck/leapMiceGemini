#define _DEFUALT_SOURCE
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "LeapC.h"
#include "xdo.h"
#include "main.h"
#include "utils.h"
#include "leapTemplate.h"

#define TRACKER_POINT         TRACKER_POINTS_INDEX_FINGER
#define INTERPOLATION_LEVEL   5
#define CLICK_SENSITIVITY     190
#define CLICK_TIMEOUT         150000
#define APP_FPS               60

LEAP_CONNECTION leapConnection;
leapHandStack_t leapHandStack;
leapCorners_t leapCorners;
screenCorners_t screenCorners;
int isRunning = 1;

#pragma region // ----------- DELTA TIME ----------- //

int64_t lastFrameTime;
int64_t deltaTime;
void updateDeltaTime(){
    int64_t currentTime = getSystemTime();
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
}

#pragma endregion

// CREATES APP DIR IF NEEDED.
// RETURNS CALIBRATION FILE DESCRIPTOR OR -1.
// IF [st] IS PROVIDED, WRITES CALIBRATION FILE STAT TO IT.
// @param mode: 1 -> WRITE, 0 -> READ
// @param st: POINTER TO STAT STRUCT FOR OUTPUT
int openCalibrationFile(int mode, struct stat *st){
    char dataDir[256];
    sprintf(dataDir,"%s%s",getenv("HOME"),"/.leapmice");
    mkdir(dataDir,0775);
    
    char calibrationFileName[256];
    sprintf(calibrationFileName,"%s%s",dataDir,"/calibration");
    int calibrationFile_fd = open(calibrationFileName,mode ? O_WRONLY||O_CREAT:O_RDONLY);
    if(calibrationFile_fd == -1) return -1;

    if(st != NULL){
        stat(calibrationFileName,st);
    }
    
    return calibrationFile_fd;
}

// READ THE CALIBRATION FILE IF IT EXISTS
// IF IT DOES, RETURN 0 AND WRITE CORNERS STRUCTS
// OTHERWISE, RETURN -1
int readCalibrationFile(leapCorners_t* leapCorners, screenCorners_t* screenCorners){
    struct stat filestat;
    int calibrationFile_fd = openCalibrationFile(0,&filestat);
    if(calibrationFile_fd == -1) return -1;

    char calibrationFileBuffer[filestat.st_size];
    int spaceCount = 0;
    for(int i=0; 1; i++){
        char c;
        int rs = read(calibrationFile_fd,&c,1);
        calibrationFileBuffer[i] = c;
        if(c == ' ' || c == '\n' || rs == 0){
            calibrationFileBuffer[i] = '\0';
            switch (spaceCount){
                // ------------ LEAP UPPER LEFT ----------- //
                case 0:  leapCorners->upperLeft.x     = atof(calibrationFileBuffer); break; //X
                case 1:  leapCorners->upperLeft.y     = atof(calibrationFileBuffer); break; //Y
                case 2:  leapCorners->upperLeft.z     = atof(calibrationFileBuffer); break; //Z
                // ------------ LEAP UPPER RIGHT ----------- //
                case 3:  leapCorners->upperRight.x    = atof(calibrationFileBuffer); break; //X
                case 4:  leapCorners->upperRight.y    = atof(calibrationFileBuffer); break; //Y
                case 5:  leapCorners->upperRight.z    = atof(calibrationFileBuffer); break; //Z
                // ------------ LEAP BOTTOM LEFT ----------- //
                case 6:  leapCorners->bottomLeft.x    = atof(calibrationFileBuffer); break; //X
                case 7:  leapCorners->bottomLeft.y    = atof(calibrationFileBuffer); break; //Y
                case 8:  leapCorners->bottomLeft.z    = atof(calibrationFileBuffer); break; //Z
                // ------------ LEAP BOTTOM RIGHT ----------- //
                case 9:  leapCorners->bottomRight.x   = atof(calibrationFileBuffer); break; //X
                case 10: leapCorners->bottomRight.y   = atof(calibrationFileBuffer); break; //Y
                case 11: leapCorners->bottomRight.z   = atof(calibrationFileBuffer); break; //Z
                // ------------ SCREEN UPPER LEFT ----------- //
                case 12: screenCorners->upperLeft.x   = atoi(calibrationFileBuffer); break; //X
                case 13: screenCorners->upperLeft.y   = atoi(calibrationFileBuffer); break; //Y
                case 14: screenCorners->screenNum     = atoi(calibrationFileBuffer); break; //SCREEN NUM
                // ------------ SCREEN UPPER RIGHT ----------- //
                case 15: screenCorners->upperRight.x  = atoi(calibrationFileBuffer); break; //X
                case 16: screenCorners->upperRight.y  = atoi(calibrationFileBuffer); break; //Y
                // ------------ SCREEN BOTTOM LEFT ----------- //
                case 18: screenCorners->bottomLeft.x  = atoi(calibrationFileBuffer); break; //X
                case 19: screenCorners->bottomLeft.y  = atoi(calibrationFileBuffer); break; //Y
                // ------------ SCREEN BOTTOM RIGHT ----------- //
                case 21: screenCorners->bottomRight.x = atoi(calibrationFileBuffer); break; //X
                case 22: screenCorners->bottomRight.y = atoi(calibrationFileBuffer); break; //Y
            }

            i = -1;
            spaceCount ++;
        }
        
        if(rs == 0){
            break;
        }

    }

    close(calibrationFile_fd);
    return 0;
}

// SAVE THE CONTENT OF [leapCorners] AND [screenCorners]
// IN THE CALIBRATION FILE
int createCalibrationFile(){
    int rs;
    int calibrationFile_fd = openCalibrationFile(1,NULL);
    if(calibrationFile_fd == -1) return -1;

    char configFileContent[255];
    sprintf(
        configFileContent,"%f %f %f\n%f %f %f\n%f %f %f\n%f %f %f\n%i %i %i\n%i %i 0\n%i %i 0\n%i %i 0\n",
        leapCorners.upperLeft.x,leapCorners.upperLeft.y,leapCorners.upperLeft.z,
        leapCorners.upperRight.x,leapCorners.upperRight.y,leapCorners.upperRight.z,
        leapCorners.bottomLeft.x,leapCorners.bottomLeft.y,leapCorners.bottomLeft.z,
        leapCorners.bottomRight.x,leapCorners.bottomRight.y,leapCorners.bottomRight.z,
        screenCorners.upperLeft.x,screenCorners.upperLeft.y,screenCorners.screenNum,
        screenCorners.upperRight.x,screenCorners.upperRight.y,
        screenCorners.bottomLeft.x,screenCorners.bottomLeft.y,
        screenCorners.bottomRight.x,screenCorners.bottomRight.y
        );


    int bufferlen = strlen(configFileContent);
    int tb = 0;
    while(1){
        int b = write(calibrationFile_fd,(void*)configFileContent,bufferlen - tb);
        if(tb >= bufferlen) break;
        if(b == -1){
            close(calibrationFile_fd);
            return 1;
        }
        tb += b;
    }
    
    close(calibrationFile_fd);
    return 0;
}

// RECEIVE A VECTOR CAPTURED BY THE LEAP MOTION AND
// CONVERT ITS POSITION TO AN SCREEN POSITION
screenCoordinates_t mapLeapVectorToScreen(LEAP_VECTOR vector, screenCorners_t* screenCorners, leapCorners_t* leapCorners){
    //FORMULA: rs = c+((x-a)*(d-c)/(b-a))
    // rs     : result
    // a -> b : old range
    // c -> d : target range
    // x      : value to map
    
    screenCoordinates_t returnV;
    float a,b,c,d,x;
    
    // X COORD
    a = leapCorners->upperLeft.x;
    b = leapCorners->bottomRight.x;
    c = (float) screenCorners->upperLeft.x;
    d = (float) screenCorners->bottomRight.x;
    x = vector.x;

    returnV.x = (int) c+((x-a)*(d-c)/(b-a));

    // Y COORD
    a = leapCorners->upperLeft.y;
    b = leapCorners->bottomRight.y;
    c = (float) screenCorners->upperLeft.y;
    d = (float) screenCorners->bottomRight.y;
    x = vector.y;

    returnV.y = (int) c+((x-a)*(d-c)/(b-a));

    //printScreenCoordinates(returnV);
    return returnV;

}

// UPDATES THE handStack WITH LAST RIGHT HANDS
// DATA IF THERE IS ANY, OTHERWISE DO NOTHING
int incomingMessageHandler(){
    LEAP_CONNECTION_MESSAGE msg;
    int result;

    result = getFrame(leapConnection,&msg);
    if(result == 0){
        LEAP_HAND* hand;
        for(int i=0; i<msg.tracking_event->nHands; i++){
            hand = msg.tracking_event->pHands+i;
            if(hand->type == eLeapHandType_Right) 
                handStack_append(&leapHandStack, hand);
        }
    }
}

int calibrationMode(){
    int rs;
    xdo_t* x = xdo_new(NULL);
    int calibrationStep = 0;
    printf("Running calibration mode\n");
    printCalibrationMsg(calibrationStep);
    int releaseFlag = 0;
    while (1){
        incomingMessageHandler();
        int ctrlK = (xdo_get_input_state(x) >> 2) & 1;
        if(ctrlK == 0){
            releaseFlag =1;
        }

        if(ctrlK == 1 && releaseFlag == 1){

            LEAP_HAND h;
            rs = handStack_getInterpolation(&leapHandStack,&h,INTERPOLATION_LEVEL);
            if(rs == 1) goto _end;

            int screen_n;
            screenCoordinates_t s;
            xdo_get_mouse_location(x,&s.x,&s.y,&screen_n);

            switch(calibrationStep){
                case screenCorners_upperLeft:
                leapCorners.upperLeft = h.TRACKER_POINT;
                screenCorners.upperLeft = s;
                screenCorners.screenNum = screen_n;
                break;

                case screenCorners_upperRight:
                leapCorners.upperRight = h.TRACKER_POINT;
                screenCorners.upperRight = s;
                break;

                case screenCorners_bottomLeft:
                leapCorners.bottomLeft = h.TRACKER_POINT;
                screenCorners.bottomLeft = s;
                break;

                case screenCorners_bottomRight:
                leapCorners.bottomRight = h.TRACKER_POINT;
                screenCorners.bottomRight = s;
                break;
            }

            printf("ok\n");
            calibrationStep++;
            if(calibrationStep == 4) break;
            printCalibrationMsg(calibrationStep);
            
            _end:;
            releaseFlag = 0;
        }

        sleepFPS(APP_FPS);
    }

    rs = createCalibrationFile();
    if(rs == -1){
        printf("calibration file could not be created : %s\n",strerror(errno));
        return 1;
    }

    printf("calibration done succesfully\n");
    return 0;
    
}

int main(int argn, char** argv){
    int rs;

#pragma region // ------------------- LEAP SERVICE SETUP ------------------- //
    LEAP_CONNECTION_MESSAGE initMsg;
    rs = LeapCreateConnection(NULL, &leapConnection);
    if(rs != eLeapRS_Success){
        printLeapResult(rs);
        return 1;
    };

    rs = LeapOpenConnection(leapConnection);
    if(rs != eLeapRS_Success){
        printLeapResult(rs);
        return 1;
    };

    rs = LeapPollConnection(leapConnection,1000,&initMsg);
    if(rs != eLeapRS_Success){
        printLeapResult(rs);
        return 1;
    };

#pragma endregion

#pragma region // ----------------- CALIBRATION FILE SETUP ----------------- //
    
    for(int i=0; i<argn; i++){
        if(strcmp(argv[i],"-c") == 0) return calibrationMode();
    }

    rs = readCalibrationFile(&leapCorners,&screenCorners);
    if(rs == -1){
        fprintf(stderr,"%s\n","There is no calibration file");
        return 1;
    }

#pragma endregion

    xdo_t* x = xdo_new(NULL);
    LEAP_HAND miceHand;
    int clickTimeOut = 0;
    while(isRunning){
        updateDeltaTime();
        incomingMessageHandler();
        rs = handStack_getInterpolation(&leapHandStack,&miceHand,INTERPOLATION_LEVEL);
        if(rs == 0){
            screenCoordinates_t mouseCoordinates = mapLeapVectorToScreen(
                miceHand.TRACKER_POINT,&screenCorners,&leapCorners);
            xdo_move_mouse(x,mouseCoordinates.x,mouseCoordinates.y,screenCorners.screenNum);
            
            if(miceHand.palm.velocity.z < CLICK_SENSITIVITY*-1){
                clickTimeOut = 1;
            }

            if(miceHand.palm.velocity.z > CLICK_SENSITIVITY && clickTimeOut > 0){
                clickTimeOut = 0;
                xdo_click_window(x,CURRENTWINDOW,1);
            }

        }

        if(clickTimeOut > 0){
            clickTimeOut += deltaTime;
            if(clickTimeOut >= CLICK_TIMEOUT){
                clickTimeOut = 0;
            }
        }

        sleepFPS(APP_FPS);
    }

    xdo_free(x);
    return 0;
}
