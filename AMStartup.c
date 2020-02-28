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
 * 
 * NOTE: The section in which I connect to the server is take from the inclient.c program we 
 *  were told we could reference
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures
#include "amazing.h"
#include "getopt.h"


/**************** main() ****************/
int
main(const int argc, char *argv[]) {
  
  char *program = argv[0];	  // this program's name
  int port = 17235;		        // server port

  int nAvatars;
  int Difficulty;
  char Hostname[100];

  int c;

/**************** parse the command line ********************/

  // parse the command line by looping until no options left
  while ((c = getopt (argc, argv, "n:d:h:")) != -1)
    switch (c)
      {
      case 'n':
        nAvatars = atoi(optarg);
        break;
      case 'd':
        Difficulty = atoi(optarg);
        break;
      case 'h':
        strcpy(Hostname, optarg);
        break;
      default:
        fprintf(stderr, "usage: ./AMStartup -n nAvatars -d Difficulty -h Hostname]\n");
        exit(1);
        break;
      }

  // to test parsing
  printf("nAvatars: %d\n", nAvatars);
  printf("Difficulty: %d\n", Difficulty);
  printf("Hostname: %s\n", Hostname);

/******************* validate parameters *********************/

  // nAvatars must be an integer less than a set value
  if ( nAvatars > AM_MAX_AVATAR ) {
    fprintf(stderr, "error: the nAvatars must be less than %d", AM_MAX_AVATAR);
    exit(2);
  }

  // Difficulty must be an integer greater than 0 and less than 10
  if ( Difficulty <0 || Difficulty >= 10 ) { 
    fprintf(stderr, "error: Difficulty must be an integer greater than 0 and less than 10");
    exit(3);
  }

  // hostname must be char* to flume.cs.dartmouth.edu
  if ( strcmp(Hostname, "flume.cs.dartmouth.edu") != 0 ) {
    fprintf(stderr, "error: the hostname must be flume.cs.dartmouth.edu");
    exit(4);
  }


/******************* connect to the server *******************/

  // create a socket
  int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (comm_sock < 0) {
    perror("opening socket");
    exit(5);
  }
  
  // Initialize the fields of the server address
  struct sockaddr_in server;  // address of the server
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  // Look up the hostname specified on command line
  struct hostent *hostp = gethostbyname(Hostname); // server hostname
  if (hostp == NULL) {
    fprintf(stderr, "%s: unknown host '%s'\n", program, Hostname);
    exit(6);
  }  
  memcpy(&server.sin_addr, hostp->h_addr_list[0], hostp->h_length);

  // connect the socket to the server
  if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("connecting stream socket");
    exit(7);
  }
  printf("Connected to server!\n");


  // create the AM_INIT message to send to the server
  AM_Message message;
  message.type = htonl(AM_INIT);
  uint32_t difficulty = Difficulty; 
  uint32_t number_of_avatars = nAvatars;
  message.init.Difficulty = ntohl(difficulty);
  message.init.nAvatars = ntohl(number_of_avatars);

  // send the message to the server 
  write(comm_sock, (void *) &message, sizeof(message));

  // listen for the AM_INTI_OK message from the server
  AM_Message response;
  int res = read(comm_sock, (void *) &response, sizeof(AM_Message));
  if (res < 0) {
    printf("nothing received");
  }

  // if received an error message
  if (IS_AM_ERROR(response.type)) {
    printf("Error recieved\n");
    exit(8);
  }

  // if initialized, recover the Mazeport, MazeHeight, and MazeWidth
  if (response.type == ntohl(AM_INIT_OK)) {
    printf("Initialized!\n");

    // store the following variables for future use
    uint32_t MazePort = ntohl(response.init_ok.MazePort);
    uint32_t MazeHeight = ntohl(response.init_ok.MazeHeight);
    uint32_t MazeWidth = ntohl(response.init_ok.MazeWidth);

    // test to see getting the right message
    printf("MazePort: %d\n", ntohl(response.init_ok.MazePort));
    printf("MazeHeight: %d\n", ntohl(response.init_ok.MazeHeight));
    printf("MazeWith: %d\n", ntohl(response.init_ok.MazeWidth));

  }
}
