CC = gcc
CFLAGS = -Wall
SRC = list.c

all: list.o
	$(CC) $(CFLAGS) list.o -o list

list.o: list.c
	$(CC) $(CFLAGS) -c list.c

clean: 
	rm -rf *.o list