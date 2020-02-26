/* 
 * AMStartup.c
 *
 * Handles the startup process, validating parameters, 
 * creating a connection to the server, creating a log 
 * file, and starting up threads with the parameters
 * they need. 
 * 
 * 
 * usage: ./AMStartup -n nAvatars -d Difficulty -h Hostname 
 * 
 * We_free (Christopher Sykes, Sebastian Saker, Ben Matejka, Jacob Werzinsky), February 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures


/**************** main() ****************/
int
main(const int argc, char *argv[])
{
  char *program;	  // this program's name
  char *hostname;	  // server hostname
  int port;		      // server port

  // Check arguments