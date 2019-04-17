CC = gcc
CFLAGS = -Wall

ma:
	$(CC) $(CFLAGS) ma.c -o ma

clean:
	rm ma artigos strings
