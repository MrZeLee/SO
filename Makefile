CC = gcc
CFLAGS = -Wall -O2

ma: ma.c ma.h
	$(CC) $(CFLAGS) ma.c -o ma

sv: sv.c
	$(CC) $(CFLAGS) sv.c -o sv

cv: cv.c
	$(CC) $(CFLAGS) cv.c -o cv

clean:
	rm ma artigos strings
