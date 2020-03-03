#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	
#include <stdbool.h>      
#include <netdb.h>	      // socket-related structures
#include <sys/socket.h> 
#include "amazing.h"

//create a new avatar struct (as defined in amazing.h)
Avatar *avatar_new(char* p, int aID, int nAv, int diff, char *host, int mPort, char* log, int sock)
{
    Avatar *avatar = malloc(sizeof(Avatar)); 
    avatar->program = p; 
    avatar->AvatarId = aID; 
    avatar->nAvatars = nAv;
    avatar->Difficulty = diff; 
    avatar->hostname = host;
    avatar->MazePort = mPort;
    avatar->logfilename = log;
    avatar->fd = sock; 
    avatar->endgame = false; 

    //if (avatar->program == NULL || avatar->AvatarId == NULL || avatar->nAvatars == NULL || avatar->Difficulty == NULL || avatar->hostname == NULL || avatar->MazePort == NULL || avatar->logfilename == NULL){
    if (avatar == NULL){
        return NULL; 
    }
    else {
        return avatar; 
    }
}

bool is_end_game(Avatar *avatar)
{
  if (avatar->endgame == true){
    //close the comm_sock
    return true; 
  }
  else {
    return false; 
  }
}
    
//helper functions to handle messages 
static bool error_msgs(AM_Message resp); 
static bool end_program(AM_Message resp); 
static bool maze_solved(AM_Message resp, Avatar *avatar); 


void* avatar_play(void *avatar_p)
{
  while(true)
  {
    printf("next");
  }
    Avatar *avatar = avatar_p; 
    int a = 1; 
    void *p = (void*)&a; 
    FILE* fp = fopen(avatar->logfilename, "a"); 
    int bytes_read;       // #bytes read from socket
    //memset(buf, 0, BUFSIZE); // clear up the buffer
    AM_Message avatar_r; 
    avatar_r.type = htonl(AM_AVATAR_READY); 
    avatar_r.avatar_ready.AvatarId = htonl(avatar->AvatarId); //send AvatarId to server 
  
    //AM_Message 
    // Sends avatar_ready to the server
    if (write(avatar->fd, (void*) &avatar_r, sizeof(avatar_r)) < 0){
      exit(5);
    }

    AM_Message avatar_play; 
    int TurnID; 
    XYPos pos_array[avatar->nAvatars];
    // receives message back from server (after avatar_ready)
    do {
      if ((bytes_read = read(avatar->fd, (void*) &avatar_play, sizeof(avatar_play))) < 0) {
        exit(5);
      } else { 
        //checks if it was successful 
        if(ntohl(avatar_play.type) == AM_NO_SUCH_AVATAR){
          fprintf(stderr,"No such avatar\n"); 
        }
        else if (ntohl(avatar_play.type) == AM_AVATAR_TURN){   //gets the TurnID from the server and the XYPOS of each of the avatars 
            TurnID = ntohl(avatar_play.avatar_turn.TurnId); 
            printf("turnid: %d\n", TurnID); 
            //FILE* fp = fopen(avatar->logfilename, "a"); 
            for (int i = 0; i < avatar->nAvatars; i++){
              pos_array[i].x = ntohl(avatar_play.avatar_turn.Pos[i].x); //might not need ntohl
              pos_array[i].y = ntohl(avatar_play.avatar_turn.Pos[i].y);
            } 
            avatar->pos.x = ntohl(avatar_play.avatar_turn.Pos[avatar->AvatarId].x);  //update the avatar struct 
            avatar->pos.y = ntohl(avatar_play.avatar_turn.Pos[avatar->AvatarId].y);
            fprintf(fp,"Inserted avatar %d at %d,%d\n",TurnID, avatar->pos.x, avatar->pos.y); 
            fprintf(fp, "avatar locations:\n"); // todo
        }
      }
    } while (bytes_read > 0);

    //if it's the avatar's turn to move
    if (avatar->AvatarId == TurnID){
      AM_Message avatar_m; 
      avatar_m.type = htonl(AM_AVATAR_MOVE);
      avatar_m.avatar_move.AvatarId = htonl(avatar->AvatarId);  
      /*
        insert algorithm function that determines the move 
        int direction = algorithm_function(); 
      */ 
      int direction = 1; // **** just for testing ****
      avatar_m.avatar_move.AvatarId = htonl(direction); 
      if (write(avatar->fd, (void*) &avatar_m, sizeof(avatar_m)) < 0){  //send message to avatar 
        exit(5);
      }

      AM_Message move_resp; 
      do {
        if ((bytes_read = read(avatar->fd, (void*) &move_resp, sizeof(move_resp))) < 0) {
          exit(5);
        } 
        else { 
          //checks if it was successful 
          if (!error_msgs(move_resp)){
            if (!end_program(move_resp) && !maze_solved(move_resp, avatar)){
              if (ntohl(avatar_play.type) == AM_AVATAR_TURN){   //gets the TurnID from the server and the XYPOS of each of the avatars 
                if (pos_array[TurnID].x == ntohl(move_resp.avatar_turn.Pos[TurnID].x) && pos_array[TurnID].y == ntohl(move_resp.avatar_turn.Pos[TurnID].y)){
                  // ******if the position of the avatar did not change, do something ***** 
                }
                pos_array[TurnID].x = ntohl(move_resp.avatar_turn.Pos[TurnID].x); //might not need ntohl
                pos_array[TurnID].y = ntohl(move_resp.avatar_turn.Pos[TurnID].y); //updates the x,y position of avatar
                avatar->pos.x = ntohl(move_resp.avatar_turn.Pos[TurnID].x);  //update the avatar struct 
                avatar->pos.y = ntohl(move_resp.avatar_turn.Pos[TurnID].y);
                TurnID = ntohl(move_resp.avatar_turn.TurnId);  //the updated TurnID  
              }
              avatar->endgame = true; 
              fclose(fp); 
              // todo: free memory 
            }
          }
        }
      } while (bytes_read > 0);
    }
    return p; 
}
static bool error_msgs(AM_Message resp)
{
  if (ntohl(resp.type) == AM_SERVER_DISK_QUOTA){
    fprintf(stderr, "disk quota error\n"); 
  }
  if(ntohl(resp.type) == AM_NO_SUCH_AVATAR){
    fprintf(stderr, "no such avatar\n"); 
    return true; 
  }
  else if (ntohl(resp.type) == AM_UNKNOWN_MSG_TYPE){
    fprintf(stderr, "unknown message type: %d\n", ntohl(resp.unknown_msg_type.BadType)); 
    return true; 
  }
  else if (ntohl(resp.type) == AM_UNEXPECTED_MSG_TYPE){
    fprintf(stderr, "unexpected message type\n"); 
    return true; 
  }
  else if (ntohl(resp.type) == AM_AVATAR_OUT_OF_TURN){
    fprintf(stderr, "avatar out of turn\n"); 
    return true; 
  }
  return false; 
}

static bool end_program(AM_Message resp)
{
  if(ntohl(resp.type) == AM_SERVER_TIMEOUT){
    fprintf(stderr, "AM Server Timeout\n"); 
    return true; 
  }
  else if (ntohl(resp.type) == AM_SERVER_OUT_OF_MEM){
    fprintf(stderr, "AM Server out of memory\n"); 
    return true; 
  }
  else if(ntohl(resp.type) == AM_TOO_MANY_MOVES){
    fprintf(stderr, "too many moves\n"); 
    return true; 
  }
  return false; 
}

static bool maze_solved(AM_Message resp, Avatar *avatar)
{
  if(ntohl(resp.type) == AM_MAZE_SOLVED){
    FILE* fp = fopen(avatar->logfilename, "a"); //change this 
    fprintf(fp, "%d, %d, %d, %d\n", ntohl(resp.maze_solved.nAvatars), ntohl(resp.maze_solved.Difficulty), ntohl(resp.maze_solved.nMoves), ntohl(resp.maze_solved.Hash)); 
    fclose(fp); 
    // todo: need to write to the log for each move (up above) --> use "a"
    return true; 
  }
  return false; 
}
