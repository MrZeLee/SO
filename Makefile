CC = gcc
CFLAGS = -Wall -O2

install: ma.c ma.h sv.c cv.c
	$(CC) $(CFLAGS) ma.c ma.h extra.c extra.h -o ma
	$(CC) $(CFLAGS) sv.c extra.h extra.c -o sv
	$(CC) $(CFLAGS) cv.c -o cv

clean:
	rm ma sv cv artigos strings
