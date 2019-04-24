CC = gcc
CFLAGS = -Wall -O2

install: ma.c ma.h sv.c cv.c
	$(CC) $(CFLAGS) ma.c -o ma
	$(CC) $(CFLAGS) sv.c -o sv
	$(CC) $(CFLAGS) cv.c -o cv

clean:
	rm ma artigos strings
