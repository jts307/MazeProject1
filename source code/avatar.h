/* avatar.h 
 * CS50 Final Project - Winter 2020 - Team We_free 
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	
#include <stdbool.h>      
#include <netdb.h>	      // socket-related structures
#include <sys/socket.h> 
#include "amazing.h"

/********* avatar_new ********
 * takes the program name, avatarID, number of avatars, level of difficulty, hostname, port number, logfilename and socket number
 *  as parameters
 * creates a new Avatar struct and initializes its stored vars to these parameters 
 * mallocs space for the new avatar struct and returns NULL if it couldnt not be malloc'd otherwise
 *  it returns the new Avatar struct 
 */ 
Avatar *avatar_new(char* p, int aID, int nAv, int diff, char *host, int mPort, char* log, int sock); 

/******** avatar_play *******
 * the main program that every thread runs 
 * takes a void pointer as the parameter and returns a void pointer 
 * interacts with the server by sending and recieving messages, computes the move for each avatar, and runs until 
 *  either the game ends or a specified error. Free's any allocated memory and closes its own socket.
 * uses many helper functions as described in README.md and in the avatar.c file 
 */
void* avatar_play(void* avatar_p); 