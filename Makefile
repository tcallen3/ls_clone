CFLAGS = -ansi -g -Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init 
CFLAGS += -Wlogical-op -Wpedantic -Wshadow

PROG = my_ls

SRC = ls.c helpers.c print.c
BIN = bin

all: ${PROG}

depend:
	mkdep -- ${CFLAGS} *.c

${PROG}: ${SRC}
	mkdir -p ${BIN}
	${CC} ${CFLAGS} -o ${BIN}/${PROG} ${SRC}

clean:
	rm -rf ${PROG}
