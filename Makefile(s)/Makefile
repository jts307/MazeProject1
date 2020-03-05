# Makefile for amazing_client 
# Final Project CS50 Winter 2020

PROGS = amazing_client
LLIBS = amazing.h 

FLAGS = # 
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: amazing_client amazing_client.o 

########### inclient ##################
amazing_client: amazing_client.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

amazing_client.o: amazing.h 

clean:
	rm -rf *~ *.o *.dSYM
	rm -f $(PROGS) 
