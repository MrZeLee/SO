CC = gcc
CFLAGS = -Wall -O2

ma: ma.c ma.h
	$(CC) $(CFLAGS) ma.c -o ma

clean:
	rm ma artigos strings
