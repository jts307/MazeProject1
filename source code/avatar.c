#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>	      // read, write, close
#include <string.h>	
#include <stdbool.h>      
#include <netdb.h>	      // socket-related structures
#include <sys/socket.h> 
#include "amazing.h"
#include <pthread.h>
#include <math.h>
#include "maze.h"
#include "graphics.h"
#include "counters.h"
#include "priority_queue.h"
#include "memory.h"
#include <unistd.h>

//create a new avatar struct (as defined in amazing.h)
Avatar *avatar_new(char* p, int aID, int nAv, int diff, char *host, int mPort, char* log, int sock, maze_t *maze, pthread_mutex_t *mutex1, pthread_mutex_t *mutex2)
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
    avatar->maze = maze;
    avatar->goals = NULL; 
    avatar->leader = -1;
    avatar->centerX = 0;
    avatar->centerY = 0;
    avatar->mutex1 = mutex1;
    avatar->mutex2 = mutex2;
    avatar->avatarsPos = NULL;

    if (avatar == NULL){
        return NULL; 
    }
    else {
        return avatar; 
    }
}

void node_print(FILE *fp, void *item);

//helper functions to handle messages 
static bool error_msgs(AM_Message resp); 
static bool end_program(AM_Message resp); 
static bool maze_solved(AM_Message resp, Avatar *avatar, FILE* fp);
static bool is_end_game(Avatar *avatar);  
static void location_writer(Avatar *avatar, FILE* fp); 
static void update_positions(Avatar *avatar, AM_Message resp); 
static void make_move(Avatar *avatar, AM_Message resp, int direction, maze_t *maze, FILE* fp); 
static void insert_avatar(Avatar *avatar, maze_t *maze, FILE* fp); 
void hashprint(FILE *fp, const char*key, void *item);
//static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
//static pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
static bool endgame;


void* avatar_play(void *avatar_p)
{
  Avatar *avatar = avatar_p; 
 // pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
 // pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t *mutex1 = avatar->mutex1;
  //pthread_mutex_t *mutex2 = avatar->mutex2;
  node_t *pervSpot = NULL;
  int a = 1; 
  void *p = (void*)&a;

  avatar->avatarsPos = count_calloc(AM_MAX_AVATAR, sizeof(XYPos));
  avatar->goals = (int*)count_calloc(avatar->nAvatars+1, sizeof(int));

  printf("initialize array:");
  for (int i=0; i <= avatar->nAvatars; i++) {
    avatar->goals[i] = 1; 
    printf("%d", avatar->goals[i]);
  }

  int port_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (port_sock < 0) {
    perror("opening socket");
    exit(9);
  }

  // Initialize the fields of the server address
  struct sockaddr_in server_address;                        // address of the server
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(avatar->MazePort);

  // connect the socket to the MazePort
  if (connect(port_sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
    perror("connecting stream socket");
    exit(10);
  }
  printf("Connected to MazePort: %d\n", avatar->MazePort);
 
  int bytes_read;       // #bytes read from socket
  AM_Message avatar_r; 
  memset(&avatar_r, 0, sizeof(avatar_r)); 
  avatar_r.type = htonl(AM_AVATAR_READY); 
  avatar_r.avatar_ready.AvatarId = htonl(avatar->AvatarId); //send AvatarId to server 
   
  // Sends avatar_ready to the server
  if (write(port_sock, (void*) &avatar_r, sizeof(avatar_r)) < 0){
    exit(5);
  } 
  AM_Message avatar_play; 
  int TurnID; 
    
  // receives message back from server (after avatar_ready)
  if ((bytes_read = read(port_sock, (void*) &avatar_play, sizeof(avatar_play))) < 0) {
    printf("err here\n"); 
    exit(5);
  } 
  else { 
    //checks if it was successful 
    if(ntohl(avatar_play.type) == AM_NO_SUCH_AVATAR){     
      fprintf(stderr,"No such avatar\n"); 
    }
    else if (ntohl(avatar_play.type) == AM_AVATAR_TURN){   //gets the TurnID from the server and the XYPOS of each of the avatars 
      TurnID = ntohl(avatar_play.avatar_turn.TurnId); 
      pthread_mutex_lock(mutex1);
      FILE* fp = fopen(avatar->logfilename, "a");   //open the logfile 
      update_positions(avatar, avatar_play); //updates the avatar positions (taken from server)
      avatar->pos.x = ntohl(avatar_play.avatar_turn.Pos[avatar->AvatarId].x);  //update the avatar struct 
      avatar->pos.y = ntohl(avatar_play.avatar_turn.Pos[avatar->AvatarId].y);

      // loop through all the avatars' x and y positions
      for (int i=0; i < avatar->nAvatars; i++) {
        // add them up
	avatar->centerX += avatar->avatarsPos[i].x; 
        avatar->centerY += avatar->avatarsPos[i].y;	
      }
      // divide by number of avatars to get average
      avatar->centerX /= avatar->nAvatars;
      avatar->centerY /= avatar->nAvatars;
   
      insert_avatar(avatar, avatar->maze, fp); // logs the avatar inserts and the locations 
      fclose(fp);
      maze_draw(avatar->maze);
      pthread_mutex_unlock(mutex1);
    }
  } 
  while (!is_end_game(avatar)){ 
    sleep(.1); 
    AM_Message move_resp;
    //if it's the avatar's turn to move
    if (avatar->AvatarId == TurnID){
      pthread_mutex_lock(mutex1); 
      FILE* fp = fopen(avatar->logfilename, "a");   //open the logfile 
      AM_Message avatar_m; 
      memset(&avatar_m, 0, sizeof(avatar_m));
      avatar_m.type = htonl(AM_AVATAR_MOVE);
      avatar_m.avatar_move.AvatarId = htonl(avatar->AvatarId);  
     
      priority_queue_t *goals = priority_queue_new();
      assertp(goals, "Failure to allocate memory for avatar's goals priority queue\n");
      int direction = -1;
      node_t *goal = NULL;

      // insert possible goal nodes into priority queue
      // with priority based off of L1 distance from avatar
      if (avatar->leader == -1) {
	// center
	int L1 = abs(avatar->centerX - avatar->pos.x) + abs(avatar->centerY - avatar->pos.y); 
	if (L1 != 0 && avatar->goals[avatar->nAvatars] == 1) {
	  priority_queue_insert(goals, get_node(avatar->maze, avatar->centerX, avatar->centerY), L1);
	} else {
	    avatar->goals[avatar->nAvatars] = 0;
	}
	// other avatars
        for (int i=0; i < avatar->nAvatars; i++) {
          if (i != avatar->AvatarId && avatar->goals[i] == 1) {
	    int otherAvaX = avatar->avatarsPos[i].x;
	    int otherAvaY = avatar->avatarsPos[i].y;
	    L1 = abs(otherAvaX - avatar->pos.x) + abs(otherAvaY - avatar->pos.y);
	    if (L1 == 0) {
		avatar->goals[i] = 0;
		if (avatar->AvatarId > i) {
		  avatar->leader = i;
		  break;
		}
	    } else {
              priority_queue_insert(goals, get_node(avatar->maze, otherAvaX, otherAvaY), L1);  
	    } 
	  }
        }
	if (avatar->leader == -1) {
	  goal = priority_queue_extract(goals);
	}
      } 
      if (avatar->leader != -1) {
          int leaderAvaX = avatar->avatarsPos[avatar->leader].x;
          int leaderAvaY = avatar->avatarsPos[avatar->leader].y;
	  goal = get_node(avatar->maze, leaderAvaX, leaderAvaY);
	  if ((leaderAvaX == avatar->pos.x) && (leaderAvaY == avatar->pos.y)) {
	    direction = M_NULL_MOVE;
	  }
      }

      if (goal != NULL && direction == -1) {
        // possible nodes to move to 
	priority_queue_t *maybeVisit = priority_queue_new();
	assertp(maybeVisit, "Failure to allocate memory for maybeVisit priority queue");
	
	// nodes to be visited during breath-first search
	priority_queue_t *toBeVisited = priority_queue_new();
	assertp(toBeVisited, "Failure to allocate memory for toBeVisited priority queue");
	
	counters_t *visited = counters_new();
	assertp(visited, "Failure to allocate memory for visited hashtable");
	node_t *start = get_node(avatar->maze, avatar->pos.x, avatar->pos.y);
	counters_add(visited, get_node_index(avatar->maze, start));
        for (int i=0; i < 4; i++) {
	  int directionState = check_node_dir(avatar->maze, start, i);
	  node_t *startNeighbor = get_neighbor(avatar->maze, start, i);
	  if(directionState == 3 || directionState == 1) {
             int unknowns=0;
	     int priority=0;
	     priority_queue_insert(toBeVisited, startNeighbor, priority++);
	     node_t *curr=NULL;
	     while ((curr=priority_queue_extract(toBeVisited)) != NULL) {
	       if (curr == goal) {
	         if (i == 0) {
		   direction = M_WEST;
		 } else if (i == 1) {
		   direction = M_NORTH;
		 } else if (i == 2) {
		   direction = M_EAST;
		 } else if (i == 3) {
		   direction = M_SOUTH;
		 }
		 break;
	       }
	       for (int j=0; j < 4; j++) {
		 if (check_node_dir(avatar->maze, curr, j) == 1) {
		     unknowns++;
		 } else if (check_node_dir(avatar->maze, curr, j) == 3) {
		       node_t *currNeighbor = get_neighbor(avatar->maze, curr, j);
	               if (counters_add(visited, get_node_index(avatar->maze, currNeighbor)) == 1) {
		         priority_queue_insert(toBeVisited, currNeighbor, priority++);
		       }
	         }
	       }
	     }
	     if (direction != -1) {
	       break;
	     }
	     if (unknowns != 0 && directionState == 3) {
	       if (pervSpot == startNeighbor) {
                 priority_queue_insert(maybeVisit, startNeighbor, get_L1_distance(startNeighbor, goal) + 1000);	       
	       } else {
                 priority_queue_insert(maybeVisit, startNeighbor, get_L1_distance(startNeighbor, goal) + 1000);	       
	       }
	     } 
	  }
	  if (directionState == 1) {
	    priority_queue_insert(maybeVisit, startNeighbor, get_L1_distance(startNeighbor, goal));
	  }
        }

        priority_queue_print(maybeVisit, stdout, node_print);
        if (direction == -1) {
	  node_t *node = priority_queue_extract(maybeVisit); 
          if (get_neighbor(avatar->maze, start, 2) == node) {
	     direction = M_EAST;
	  } else if (get_neighbor(avatar->maze, start, 0) == node) {
	     direction = M_WEST;
	  } else if (get_neighbor(avatar->maze, start, 1) == node) {
	     direction = M_NORTH;
	  } else if (get_neighbor(avatar->maze, start, 3) == node) {
	     direction = M_SOUTH;
	  }
	}
        priority_queue_delete(maybeVisit, NULL);	
	priority_queue_delete(toBeVisited, NULL);
	counters_delete(visited);
      } else {
          direction = M_NULL_MOVE;
      }
      priority_queue_delete(goals, NULL);
      avatar_m.avatar_move.Direction = htonl(direction); 

      if (write(port_sock, (void*) &avatar_m, sizeof(avatar_m)) < 0){  //send message to avatar 
        exit(5);
      }
      //AM_Message move_resp; 
        if ((bytes_read = read(port_sock, (void*) &move_resp, sizeof(move_resp))) < 0) {
          exit(5);
        } 
        else { 
          //checks if it was successful 
          if (!error_msgs(move_resp)){
            if (!end_program(move_resp) && !maze_solved(move_resp, avatar,fp)){
              if (ntohl(move_resp.type) == AM_AVATAR_TURN){   //gets the TurnID from the server and the XYPOS of each of the avatars   
                pervSpot = get_node(avatar->maze, avatar->pos.x, avatar->pos.y);
		make_move(avatar, move_resp, direction, avatar->maze, fp); // avatar moves or hits a wall, logs its movement, and updates positions
		maze_draw(avatar->maze);
		location_writer(avatar, fp); // positions are logged 
                TurnID = ntohl(move_resp.avatar_turn.TurnId);  //the updated TurnID 
		fclose(fp);
		pthread_mutex_unlock(mutex1);
              }
            }
            else {
	      maze_draw(avatar->maze);
              printf("the game is over\n");
              endgame = true;
	      maze_delete(avatar->maze); 
              fclose(fp);
              close(port_sock);
              break; 
            }
          }
        }
    }
    else {  //if the avatarid != turnid, get the updated turnid 
	if ((bytes_read = read(port_sock, (void*) &move_resp, sizeof(move_resp))) < 0) {
          exit(5);
          } 
        else { 
          if (!error_msgs(move_resp)){
            if (!end_program(move_resp)){
              if (ntohl(avatar_play.type) == AM_AVATAR_TURN){ 
                update_positions(avatar, move_resp); 
                TurnID = ntohl(move_resp.avatar_turn.TurnId);  //the updated TurnID  
              } 
            }
          }
        }
    } 
  }
  free(avatar->avatarsPos);
  free(avatar->goals);
  free(avatar);
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

static bool maze_solved(AM_Message resp, Avatar *avatar, FILE* fp)
{
  if(ntohl(resp.type) == AM_MAZE_SOLVED){	  
    printf("it's solved\n"); 
    fprintf(fp, "%d, %d, %d, %d\n", ntohl(resp.maze_solved.nAvatars), ntohl(resp.maze_solved.Difficulty), ntohl(resp.maze_solved.nMoves), ntohl(resp.maze_solved.Hash));
    return true; 
  }
  return false; 
}

static bool is_end_game(Avatar *avatar)
{
  if (endgame == true){
    return true; 
  }
  else {
    return false; 
  }
}

static void location_writer(Avatar *avatar, FILE* fp)
{
  fprintf(fp, "Avatar locations: ");
  for (int i = 0; i < avatar->nAvatars; i++){
    fprintf(fp,"%d: (%d, %d); ",i, avatar->avatarsPos[i].x, avatar->avatarsPos[i].y); 
  } 
  fprintf(fp,"\n"); 
}

static void insert_avatar(Avatar *avatar, maze_t* maze, FILE* fp)
{
  fprintf(fp,"Inserted avatar %d at %d,%d\n",avatar->AvatarId, avatar->pos.x, avatar->pos.y); 
  fprintf(fp,"Avatar locations: ");
  for (int i = 0; i < avatar->AvatarId+1; i++){
    fprintf(fp,"%d: (%d, %d); ",i, avatar->avatarsPos[i].x, avatar->avatarsPos[i].y); 
  } 
  fprintf(fp,"\n");
  set_avatar(get_node(maze, avatar->pos.x, avatar->pos.y), avatar->AvatarId);
}

static void update_positions(Avatar *avatar, AM_Message resp)
{
  for (int i = 0; i < avatar->nAvatars; i++){
    avatar->avatarsPos[i].x = ntohl(resp.avatar_turn.Pos[i].x);
    avatar->avatarsPos[i].y = ntohl(resp.avatar_turn.Pos[i].y);
  } 
}

static void make_move(Avatar *avatar, AM_Message resp, int direction, maze_t *maze, FILE* fp)
{
  int tempX = avatar->pos.x; 
  int tempY = avatar->pos.y;



  int dir = 0;
  for (int i = 0; i < avatar->nAvatars; i++){
    if (i == avatar->AvatarId){
      avatar->pos.x = ntohl(resp.avatar_turn.Pos[avatar->AvatarId].x); 
      avatar->pos.y = ntohl(resp.avatar_turn.Pos[avatar->AvatarId].y); 
      avatar->avatarsPos[i].x = avatar->pos.x; 
      avatar->avatarsPos[i].y = avatar->pos.y; 
    }
    else {
      avatar->avatarsPos[i].x = ntohl(resp.avatar_turn.Pos[i].x);
      avatar->avatarsPos[i].y = ntohl(resp.avatar_turn.Pos[i].y);
    }
  }
  // storing what direction avatar went
  char *wallDir=NULL;        
  if (direction == M_NORTH) {
      wallDir="north";
      dir = 1;
  } else if (direction == M_SOUTH) {
      wallDir="south";
      dir = 3;
  } else if (direction == M_EAST) {
      wallDir="east";
      dir = 2;
  } else if (direction == M_WEST) {
      wallDir="west";
      dir = 0;
  }
  if (direction == M_NULL_MOVE) {
    fprintf(fp, "Avatar %d stayed at %d,%d\n",avatar->AvatarId, avatar->pos.x, avatar->pos.y);
  } else if (tempX == avatar->pos.x && tempY == avatar->pos.y){
    // ******if the position of the avatar did not change, do something *****
    fprintf(fp, "avatar %d ran into %s wall at %d,%d\n", avatar->AvatarId, wallDir, avatar->pos.x, avatar->pos.y);
    set_wall(get_node(maze, tempX, tempY), maze, dir);
  }
  else {
    fprintf(fp, "Avatar %d moved from %d,%d to %d,%d\n",avatar->AvatarId, tempX, tempY, avatar->pos.x, avatar->pos.y);
    set_connection(get_node(maze, tempX, tempY), maze, dir);
    // setting avatar position in maze struct
    set_avatar(get_node(maze, avatar->pos.x, avatar->pos.y), avatar->AvatarId);
    // deleting old position
    set_avatar(get_node(maze, tempX, tempY), -1);
    // setting ally position if necessary
    for (int i=0; i < avatar->nAvatars; i++) {
      if (avatar->avatarsPos[i].x == tempX && avatar->avatarsPos[i].y == tempY) {
        set_avatar(get_node(maze, tempX, tempY), i);
      }
    }
  } 

  if (avatar->pos.x == avatar->centerX && avatar->pos.y == avatar->centerY) {
    avatar->goals[avatar->nAvatars] = 0;
  }

  for (int i=0; i < avatar->nAvatars; i++) {
    printf("x=%d, y=%d", avatar->avatarsPos[i].x, avatar->avatarsPos[i].y);
    if ((avatar->pos.x == avatar->avatarsPos[i].x) && (avatar->pos.y == avatar->avatarsPos[i].y)) {
      avatar->goals[i] = 0;
      if (avatar->AvatarId > i) {
        avatar->leader = i;
        break;
      }
    }
  }
}


void node_print(FILE *fp, void *item) {
   node_t *node=item;

   fprintf(fp, "{x=%d, y=%d}", get_node_x(node), get_node_y(node));
}

void hashprint(FILE *fp, const char*key, void *item) {
   fprintf(fp, "%s", key);
}
