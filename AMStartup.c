/* 
 * inclient - an example Internet client.
 *
 * Makes a connection to the given host/port and sends a message 
 * to that socket.
 * 
 * usage: inclient hostname port
 * 
 * David Kotz, 1987, 1992, 2016
 * Adapted from Figure 7a in Introductory 4.3bsd IPC, PS1:7-15.
 *
 * updated by Xia Zhou, August 2016, 2017, 2018
 * updated by Temi Prioleau, 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures
