CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: life

universe.o: universe.c universe.h
	$(CC) -c universe.c

life: universe.o life.o
	$(CC) -o life universe.o life.o -lm -lncurses

life.o: life.c
	$(CC) $(CFLAGS) -c life.c

clean:
	rm -rf main main.o infer-out

infer:
	make clean; infer-capture -- make; infer-analyze -- make


