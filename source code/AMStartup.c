/* 
 * AMStartup.c
 *
 * Handles the startup process, validating parameters, 
 * creating a connection to the server, creating a log 
 * file, and starting up threads with the parameters
 * they need. 
 * 
 * usage: ./AMStartup -n nAvatars -d Difficulty -h Hostname 
 * 
 * We_free (Christopher Sykes, Sebastian Saker, Ben Matejka, Jacob Werzinsky), February 2020
 * 
 * NOTE: The sections in which I connect to the server and MazePort are taken from the inclient.c 
 * program that we were told we could reference
 * 
 * Possible failures:
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	          // read, write, close
#include <string.h>	          // memcpy, memset
#include <netdb.h>	          // socket-related structures
#include <time.h>             // to find the time for log file
#include <pthread.h>          // for threads
#include "amazing.h"          // for communicating with server
#include "getopt.h"           // to parse command line by options
#include "avatar.h"           // contains function to store avatar info

/**************** file-local constants ****************/
#define BUFSIZE 1024     // read/write buffer size

/**************** main() ****************/
/* Purpose: 
 *  There is only one function in AMStartup.c, 
 *  and that is main(). Main validates parameters, 
 *  creates a connection to the server, creates a 
 *  log file, and starts up threads, passing them 
 *  the parameters that they need. 
 * 
 * Input:
 *  This function takes its input from the command
 *  line. See usage for more info
 * 
 * Output:
 *  A log file containg info about the outcome of 
 *  the avatars in the maze.
 * 
 * Possible Failures:
 *  Not aware of any. Program will exit if invalid
 *  parameter is passed.
 */
int
main(const int argc, char *argv[]) {
  
  char *program = argv[0];	  // this program's name
  int port = 17235;		        // server port

  int nAvatars;
  int Difficulty;
  char Hostname[100];         // the server will be running on flume.cs.dartmouth.edu 

  int c;

/************************ parse the command line **************************/

  // parse the command line by looping until no options left
  while ((c = getopt (argc, argv, "n:d:h:")) != -1)
    switch (c)
      {
      // number of avatars
      case 'n':
        nAvatars = atoi(optarg);
        break;
      // difficulty
      case 'd':
        Difficulty = atoi(optarg);
        break;
      // hostname
      case 'h':
        strcpy(Hostname, optarg);
        break;
      default:
        fprintf(stderr, "usage: ./AMStartup -n nAvatars -d Difficulty -h Hostname]\n");
        exit(1);
        break;
      }

/************************** validate parameters **************************/

  // nAvatars must be an integer less than a set value
  if ( nAvatars > AM_MAX_AVATAR ) {
    fprintf(stderr, "error: the nAvatars must be less than %d", AM_MAX_AVATAR);
    exit(2);
  }

  // Difficulty must be an integer greater than 0 and less than 10
  if ( Difficulty < 0 || Difficulty >= 10 ) { 
    fprintf(stderr, "error: Difficulty must be an integer greater than 0 and less than 10");
    exit(3);
  }

  // hostname must be char* to flume.cs.dartmouth.edu
  if ( strcmp(Hostname, "flume.cs.dartmouth.edu") != 0 ) {
    fprintf(stderr, "error: the hostname must be flume.cs.dartmouth.edu");
    exit(4);
  }

/************************** connect to the server *************************/

  // create a socket
  int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (comm_sock < 0) {
    perror("opening socket");
    exit(5);
  }
  
  // Initialize the fields of the server address
  struct sockaddr_in server;                        // address of the server
  server.sin_family = AF_INET;
  server.sin_port = htons(port);                    // connect to the port

  // Look up the hostname specified on command line
  struct hostent *hostp = gethostbyname(Hostname);  // server hostname
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
  memset(&message, 0, sizeof(message));                   // clear the block of memory
  message.type = htonl(AM_INIT);
  uint32_t difficulty = Difficulty; 
  uint32_t number_of_avatars = nAvatars;
  message.init.Difficulty = ntohl(difficulty);            // use ntohl to convert the bytes
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
  }
  else {
    fprintf(stderr, "Not able to intialize program");
  }

  // store the following variables for future use
  uint32_t MazePort = ntohl(response.init_ok.MazePort);
  uint32_t MazeHeight = ntohl(response.init_ok.MazeHeight);
  uint32_t MazeWidth = ntohl(response.init_ok.MazeWidth);

  /*********************** create a log file ************************/

  char *user_name = getenv("USER");

  char logfile[100];                      // an array for the name of the logfile

  // create path to file with the name in the correct format
  sprintf(logfile, "Amazing_$%s_%d_%d.log", user_name, nAvatars, Difficulty);

  // create the file to write to 
  FILE *fp = fopen(logfile, "w");

  // get time and date 
  time_t current_time;
  time(&current_time);

  // write the required info to the first line of the log file
  fprintf(fp, "$%s %d %s", user_name, MazePort, ctime(&current_time));

  fclose(fp);
  
  /************************** create threads ***************************/
  
  // first close the old socket
  close(comm_sock);

  pthread_t avatars[nAvatars];    // an array of threads
  int num_threads = 0;            // keep track of number of threads
  int iret1;
  Avatar *avatar; 

  for ( int i = 0; i < nAvatars; i++ ) {

    // initialize the contents of the avatar struct
    avatar = avatar_new(program, i, nAvatars, Difficulty, Hostname, MazePort, logfile, comm_sock, MazeHeight, MazeWidth);

    // create a thread for the avatar
    iret1 = pthread_create(&avatars[i], NULL, avatar_play, avatar);
    num_threads++;    
    sleep(1);
    
    // check to see that the thread was created
    if ( iret1 != 0 ) {
      printf("pthread_create failed");
      free(avatar); 
      exit(iret1);
    }
  }

  // detatch threads to get rid of warning in valgrind 
  for ( int i = 0; i < nAvatars; i++ ) {
    if ( pthread_detach(avatars[i]) == 0 ) {
    }
    else {
      fprintf(stderr, "error: could not detatch avatar thread");
    }
  }

// Makes sure that the main thread doesn't finish until the avatar threads do
pthread_exit(0);
exit(0); 
}
