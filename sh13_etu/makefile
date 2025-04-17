CC = gcc
CFLAGS = -Wall -g
SDLFLAGS = `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lpthread

all: sh13 server

sh13: sh13.c
	$(CC) $(CFLAGS) sh13.c -o sh13 $(SDLFLAGS)

server: server.c
	$(CC) $(CFLAGS) server.c -o server

clean:
	rm -f sh13 server *.o
