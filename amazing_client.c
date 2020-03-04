/* 
 * amazing_client.c  -- avatar startup program 
 * inspired by inclient.c 
 * project-maze-challenge CS50 Winter 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	      // memcpy, memset
#include <netdb.h>	      // socket-related structures
#include <sys/socket.h> 
#include "amazing.h"

/**************** file-local constants ****************/
#define BUFSIZE 1024     // read/write buffer size

/**************** main() ****************/
int
main(const int argc, char *argv[])
{
  char *program;	  // this program's name
  int AvatarId;
  int nAvatars; 
  int Difficulty; 
  char *hostname;     // server hostname
  int MazePort;		      // server port
  char* logfilename; 

  // 0. Check arguments
  program = argv[0];
  if (argc != 7) {
    fprintf(stderr, "usage: %s wrong number of arguments\n", program);
    exit(1);
  } else {
      AvatarId = atoi(argv[1]); 
      nAvatars = atoi(argv[2]); 
      Difficulty = atoi(argv[3]);  
      hostname = argv[4];
      MazePort = atoi(argv[5]);
      logfilename = argv[6];                                                                                                                                                              
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
  server.sin_port = htons(MazePort);
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
     // a buffer for reading data from stdin
  int bytes_read;       // #bytes read from socket
  //memset(buf, 0, BUFSIZE); // clear up the buffer
  AM_Message avatar_r; 
  avatar_r.type = htonl(AM_AVATAR_READY); 
  //uint32_t avatar_id = AvatarId; 
  //avatar_r.avatar_ready.AvatarId = htonl(AvatarId); 
  
  //AM_Message 
  // Sends avatar_ready to the server
  if (write(comm_sock, (void*) &avatar_r, sizeof(avatar_r)) < 0){
      exit(5);
  }

  AM_Message avatar_play; 
  int TurnID; 
  XYPos Pos[nAvatars];
  // receives message back from server (after avatar_ready)
  do {
    if ((bytes_read = read(comm_sock, (void*) &avatar_play, sizeof(avatar_play))) < 0) {
      perror("reading from stdin");
      exit(5);
    } else { 
        //checks if it was successful 
        if(ntohl(avatar_play.type) == AM_NO_SUCH_AVATAR){
          printf("failure\n"); 
        }
        else{   //gets the TurnID from the server and the XYPOS of each of the avatars 
            TurnID = ntohl(avatar_play.avatar_turn.TurnId); 
            printf("turnid: %d\n", TurnID); 
            for (int i = 0; i < nAvatars; i++){
              Pos[i] = avatar_play.avatar_turn.Pos[i]; 
              printf("%d", (int) sizeof((Pos))); 
            }
        }
        /*
        if(error == AvatarId){
          printf("Avatar Ready FAILED\n"); 
        }
        */ 
    
    }
  } while (bytes_read > 0); 
  close(comm_sock);
  return 0;
}