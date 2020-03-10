# Makefile for 'bag' module
#
# David Kotz, April 2016, 2017
# updated by Xia Zhou, July 2016, 2018, January 2019
# updated by Temi Prioleau, January 2020

PROG = AMStartup
OBJS = graphics.o maze.o priority_queue.o avatar.o AMStartup.o
LIBS = libcs50/libcs50.a

# uncomment the following to turn on verbose memory logging
# TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb -lm -lpthread $(TESTING) -I./libcs50
CC = gcc
MAKE = make

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

AMStartup.o: libcs50/memory.h avatar.h amazing.h
avatar.o: libcs50/memory.h amazing.h maze.h graphics.h libcs50/hashtable.h priority_queue.h avatar.h
graphics.o: maze.h graphics.h
maze.o: libcs50/memory.h maze.h
priority_queue.o: libcs50/memory.h priority_queue.h

.PHONY: test clean

test: $(PROG) test_names
	./$(PROG) < test_names

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f stocks

