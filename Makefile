CFLAGS=-Wall -Wextra -std=c11 -pedantic -O3 `pkg-config --cflags sdl2`
LIBS=`pkg-config --libs sdl2` -lm

n-body: main.c vec.h
	$(CC) $(CFLAGS) -o n-body ./main.c $(LIBS)
