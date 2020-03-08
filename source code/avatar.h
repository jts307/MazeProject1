#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	
#include <stdbool.h>      
#include <netdb.h>	      // socket-related structures
#include <sys/socket.h> 
#include "amazing.h"

Avatar *avatar_new(char* p, int aID, int nAv, int diff, char *host, int mPort, char* log, int sock, maze_t *maze, pthread_mutex_t *mutex1, pthread_mutex_t *mutex2, bool *endgame); 

bool is_end_game(Avatar *avatar); 

void* avatar_play(void*); 
