CC=gcc
CFLAGS= 
LDFLAGS=-L/lib/ultraleap-hand-tracking-service -lm -lLeapC -lxdo

SRC_DIR=src
OBJ_DIR=obj
OUTPUT_DIR=bin

SRC=main utils leapTemplate
OUTPUT=leapMice

all: ${OUTPUT} ${CALIBRATOR_OUTPUT}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c
	${CC} -c -Iinclude -o $@ $<
	
${OUTPUT}:${SRC:%=${OBJ_DIR}/%.o}
	${CC} ${CFLAGS} ${LDFLAGS} -o ${OUTPUT_DIR}/${OUTPUT} ${SRC:%=${OBJ_DIR}/%.o}

clean:
	rm -f obj/* bin/*

clear: clean