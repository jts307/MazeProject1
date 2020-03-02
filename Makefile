# Makefile for AMStartup.c
#
# Christopher Sykes, February 2020

PROG = AMStartup
LIBS = 
LLIBS = amazing.h 
LDFLAGS = -pthread

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -I$L -I$C
CC = gcc
MAKE = make

all: $(PROG)

querier: querier.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ $(LDFLAGS)

AMStartup.o: amazing.h 


.PHONY: valgrind clean


############ valgrind ##################
valgrind: myvalgrind $(PROG)
	myvalgrind ./$(PROG)


################ clean #################
clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f stocks
	rm -f Amazing_*
