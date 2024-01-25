CC=gcc
FLAGS=-Wall -g
OUTPUT=memory_management.out

run: build
	./${OUTPUT}

build: memory_management.c
	${CC} ${FLAGS} -o ${OUTPUT} $^

clean:
	rm -f ${OUTPUT}