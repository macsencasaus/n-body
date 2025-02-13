CFLAGS=-Wall -Wextra -std=c11 -pedantic -O3 `pkg-config --cflags sdl2`
LIBS=`pkg-config --libs sdl2` -lm

tb: main.c
	$(CC) $(CFLAGS) -o tb ./main.c $(LIBS)
