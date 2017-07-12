CC = gcc
CFLAGS =-O3 -Wall -Wextra -pedantic -std=gnu11
LDFLAGS=-lsodium

.PHONY: clean all

all : dimacs

rebuild : clean all

dimacs : dimacs.c dimacs.h 
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean :
	rm -rf *.o dimacs
