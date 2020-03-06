# Makefile for 'bag' module
#
# David Kotz, April 2016, 2017
# updated by Xia Zhou, July 2016, 2018, January 2019
# updated by Temi Prioleau, January 2020

PROG = graphics
OBJS = graphics.o libcs50/memory.o maze.o 
LIBS =

# uncomment the following to turn on verbose memory logging
# TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -Ilibcs50
CC = gcc
MAKE = make

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

graphics.o: maze.h
maze.o: libcs50/memory.h maze.h

.PHONY: test clean

test: $(PROG) test_names
	./$(PROG) < test_names

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f stocks

