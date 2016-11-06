CC = gcc
CFLAGS = -Wall
SRC = main.c hash.c
OBJ = main.o hash.o

all: main.o hash.o
	$(CC) $(CFLAGS) $(OBJ) -o hash

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

hash.o: hash.c
	$(CC) $(CFLAGS) -c hash.c

clean: 
	rm -rf *.o hash