CC=gcc
CFLAGS= 
LDFLAGS=-L/lib/ultraleap-hand-tracking-service -lm -lLeapC -lxdo

SRC_DIR=src
OBJ_DIR=obj
OUTPUT_DIR=bin
INSTALL_DIR=/usr/local/bin/

SRC=main utils leapTemplate
OUTPUT=leapMice

all: ${OUTPUT}

${OBJ_DIR}:
	mkdir -p ${OBJ_DIR}

${OUTPUT_DIR}:
	mkdir -p ${OUTPUT_DIR}

${OBJ_DIR}/%.o:${SRC_DIR}/%.c ${OBJ_DIR}
	${CC} -c -Iinclude -o $@ $<
	
${OUTPUT}:${SRC:%=${OBJ_DIR}/%.o} ${OUTPUT_DIR}
	${CC} ${CFLAGS} ${LDFLAGS} -o ${OUTPUT_DIR}/${OUTPUT} ${SRC:%=${OBJ_DIR}/%.o}

install: all
	cp ${OUTPUT_DIR}/${OUTPUT} ${INSTALL_DIR}${OUTPUT}

uninstall:
	rm -f ${INSTALL_DIR}${OUTPUT}
	rm -fr $(HOME)/.leapmice/

clean:
	rm -f obj/* bin/*

clear: clean
