CC = gcc
CFLAGS = -Wall -O2

install: ma.c ma.h sv.c cv.c ag.c
	$(CC) $(CFLAGS) ma.c ma.h extra.c extra.h -o ma
	$(CC) $(CFLAGS) sv.c extra.h extra.c -o sv
	$(CC) $(CFLAGS) cv.c -o cv
	$(CC) $(CFLAGS) ag.c -o ag

clean:
	rm ma sv cv ag artigos strings stock vendas precos quantidade

tmp:
	rm /tmp/server*
	rm /tmp/client*
	rm /tmp/exits
	rm /tmp/in*
