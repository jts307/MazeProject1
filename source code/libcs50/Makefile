# Makefile for CS50 library
#   Builds and compiles all these utility functions into a library.
#
# If you prefer our data-structure implementation over your own, modify 
# the rule for `$(LIB)`:
# $(LIB): libcs50-given.a
#	cp libcs50-given.a $(LIB)
#
# David Kotz, April 2016, July 2017, April 2019
# Updated by Xia Zhou, July 2016
# Updated by Temi Prioleau, January 2020

# object files, and the target library
OBJS = bag.o counters.o file.o hashtable.o jhash.o memory.o set.o webpage.o
LIB = libcs50.a

# add -DNOSLEEP to disable the automatic sleep after web-page fetches
#               (students, please do not use -DNOSLEEP!)
# add -DMEMTEST for memory tracking report in indexer
# (and run `make clean; make` whenever you change this)
FLAGS = # -DMEMTEST  # -DNOSLEEP

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make


$(LIB): libcs50-given.a
	cp libcs50-given.a $(LIB)

# Build the library by archiving object files
#$(LIB): $(OBJS)
#	ar cr $(LIB) $(OBJS)

# Dependencies: object files depend on header files
bag.o: bag.h
counters.o: counters.h
file.o: file.h
hashtable.o: hashtable.h set.h jhash.h 
jhash.o: jhash.h
memory.o: memory.h
set.o: set.h
webpage.o:  webpage.h

.PHONY: clean sourcelist

# list all the sources and docs in this directory.
# (this rule is used only by the Professor in preparing the starter kit)
sourcelist: Makefile *.md *.c *.h
	@echo $^

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o
