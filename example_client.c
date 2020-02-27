/* 
 * getting used to socket/server connection to use for AMStartup.c and amazing_client.c 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures
#include "amazing.h"

/**************** file-local constants ****************/
#define BUFSIZE 1024     // read/write buffer size

/**************** main() ****************/
int
main(const int argc, char *argv[])
{
  char *program;	  // this program's name
  char *hostname;	  // server hostname
  int port;		      // server port

  // 0. Check arguments
  program = argv[0];
  if (argc != 3) {
    fprintf(stderr, "usage: %s hostname port\n", program);
    exit(1);
  } else {
    hostname = argv[1];
    port = atoi(argv[2]);
  }

  // 1. Create socket
  int comm_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (comm_sock < 0) {
    perror("opening socket");
    exit(2);
  }

  // 2. Initialize the fields of the server address
  struct sockaddr_in server;  // address of the server
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  // Look up the hostname specified on command line
  struct hostent *hostp = gethostbyname(hostname); // server hostname
  if (hostp == NULL) {
    fprintf(stderr, "%s: unknown host '%s'\n", program, hostname);
    exit(3);
  }  
  memcpy(&server.sin_addr, hostp->h_addr_list[0], hostp->h_length);

  // 3. Connect the socket to that server   
  if (connect(comm_sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
    perror("connecting stream socket");
    exit(4);
  }
  printf("Connected!\n");

  // 4. Read content from stdin (file descriptor = 0) and write to socket
  //char buf[BUFSIZE];    // a buffer for reading data from stdin
  int bytes_read;       // #bytes read from socket
  //memset(buf, 0, BUFSIZE); // clear up the buffer
  //struct init_ok *init; 
  AM_Message initmess; 
  initmess.init.nAvatars = htonl(3); 
  initmess.init.Difficulty = htonl(2); 
  //char* buf = (char*)&initmess; 
  int MazePort; 
  int MazeWidth; 
  int MazeHeight; 


  //AM_Message 

  if (write(comm_sock, &initmess, sizeof(initmess)) < 0){
      perror("reading from stdin");
      exit(5);
  }

  AM_Message initokmess; 
  //char* buf2 = (char*)&initokmess; 
  do {
    if ((bytes_read = read(comm_sock, &initokmess, sizeof(initokmess))) < 0) {
      perror("reading from stdin");
      exit(5);
    } else {
      
        if(IS_AM_ERROR(ntohl(initokmess.init_failed.ErrNum))){
          int err = ntohl(initokmess.init_failed.ErrNum); 
          printf("%d\n", err); 
           printf("%d\n", ntohl(AM_INIT_BAD_DIFFICULTY)); 
          if (ntohl(AM_INIT_ERROR_MASK) == err){
            printf("%d\n", ntohl(AM_INIT_ERROR_MASK)); 
            printf("errormask\n"); 
          }
          if (ntohl(AM_INIT_TOO_MANY_AVATARS) == err){
            printf("too many errors\n"); 
          }
          if (ntohl(AM_INIT_BAD_DIFFICULTY)  == err){
            printf("bad difficulty\n"); 
          }

          printf("AM_INIT FAILED\n"); 
        }


       MazePort = ntohl(initokmess.init_ok.MazePort); 
       MazeHeight = ntohl(initokmess.init_ok.MazeHeight);
       MazeWidth = ntohl(initokmess.init_ok.MazeWidth);

       printf("%d\n", MazePort); 
       printf("%d\n", MazeHeight);
       printf("%d\n", MazeWidth);
      //AM_Message.init_ok.MazePort; 
      //AM_Message init = 
      //printf("here is the message: %s/n", ntohl(buf)); 
    }
  } while (bytes_read > 0);
  printf("maybe here?\n"); 
  close(comm_sock);
  printf("maybe here or here ?\n");
  return 0;
}
