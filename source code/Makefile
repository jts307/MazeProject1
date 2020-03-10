<<<<<<< HEAD
# Makefile for AMStartup module
# CS50 Final Project
# Team we_free 

=======
# Makefile for 'AMStartup' program
#
# we_free, cs50, Winter 2020
>>>>>>> f3e5930bece396a89b894c8a03c31bdd50ba9ac8

PROG = AMStartup
OBJS = graphics.o maze.o priority_queue.o avatar.o AMStartup.o
LIBS = libcs50/libcs50.a

# uncomment the following to turn on verbose memory logging
# TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb -lm -lpthread $(TESTING) -I./libcs50
CC = gcc
MAKE = make

$(PROG): $(OBJS)
	make -C libcs50
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

AMStartup.o: libcs50/memory.h avatar.h amazing.h
avatar.o: libcs50/memory.h amazing.h maze.h graphics.h libcs50/hashtable.h priority_queue.h avatar.h
graphics.o: maze.h graphics.h
maze.o: libcs50/memory.h maze.h
priority_queue.o: libcs50/memory.h priority_queue.h

.PHONY: clean

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f stocks
