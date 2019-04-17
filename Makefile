CC = gcc
CFLAGS = -Wall

ma: ma.c ma.h
	$(CC) $(CFLAGS) ma.c -o ma

clean:
	rm ma artigos strings
