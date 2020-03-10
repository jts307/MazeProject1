/* avatar.c - 'avatar' module
 *
 * see avatar.h for more details
 *
 * we_free, CS50, March 2020
 */

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
    avatar->back_tracked = 0;
    avatar->trap = 0;

    if (avatar == NULL) {
        return NULL; 
    } else {
        return avatar; 
    }
}

static bool error_msgs(AM_Message resp); 
static bool end_program(AM_Message resp); 
static bool maze_solved(AM_Message resp, Avatar *avatar, FILE* fp);
static bool is_end_game(void);  
static void location_writer(Avatar *avatar, FILE* fp); 
static void update_positions(Avatar *avatar, AM_Message resp); 
static void make_move(Avatar *avatar, AM_Message resp, int direction, maze_t *maze, FILE* fp); 
static void insert_avatar(Avatar *avatar, maze_t *maze, FILE* fp); 

static bool endgame;          // is the current game over
static int num_of_moves;      // the number of moves made so far

void* avatar_play(void *avatar_p)
{
  Avatar *avatar = avatar_p; 
  pthread_mutex_t *mutex1 = avatar->mutex1;
  int a = 1; 
  void *p = (void*)&a;

  // the pervious position in the maze the avatar is in.
  node_t *pervSpot = NULL;

  // number of moves made so far
  num_of_moves = 0;

  // allocating space for avatar variables
  avatar->avatarsPos = count_calloc(AM_MAX_AVATAR, sizeof(XYPos));
  avatar->goals = (int*)count_calloc(avatar->nAvatars+1, sizeof(int));

  // setting all goals of avatar to valid
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
      // divide by number of avatars to get center point of avatars
      avatar->centerX /= avatar->nAvatars;
      avatar->centerY /= avatar->nAvatars;
   
      // log the avatar insert and its location
      insert_avatar(avatar, avatar->maze, fp);
      fclose(fp);

      // draw the maze structure
      maze_draw(avatar->maze);

      pthread_mutex_unlock(mutex1);
    }
  } 
  // continue until one of the end conditions is satisfied
  while (!is_end_game()) { 
    sleep(.1); 
    AM_Message move_resp;

    //if it's the avatar's turn to move
    if (avatar->AvatarId == TurnID){
      pthread_mutex_lock(mutex1);

      // open the logfile for appending
      FILE* fp = fopen(avatar->logfilename, "a");  
      
      AM_Message avatar_m; 
      memset(&avatar_m, 0, sizeof(avatar_m));
      avatar_m.type = htonl(AM_AVATAR_MOVE);
      avatar_m.avatar_move.AvatarId = htonl(avatar->AvatarId);  
     
      // priority queue containing potential goals an avatar can have
      priority_queue_t *goals = priority_queue_new();
      assertp(goals, "Failure to allocate memory for avatar's goals priority queue\n");
      
      // the direction the avatar will move this turn
      int direction = -1;

      // the goal an avatar will try to move towards
      node_t *goal = NULL;

      // insert possible goals into priority queue
      // with priority based off of their L1 distance from this avatar
      if (avatar->leader == -1) {
	// first consider the center
	int L1 = abs(avatar->centerX - avatar->pos.x) + abs(avatar->centerY - avatar->pos.y); 
	
	// if the avatar is not in the same place as the center and the center is a valid goal
	if ((L1 != 0) && (avatar->goals[avatar->nAvatars] == 1)) {
          // insert it into the goal queue
	  priority_queue_insert(goals, get_node(avatar->maze, avatar->centerX, avatar->centerY), L1);
	// otherwise set the center as an invalid goal
	} else {
	    avatar->goals[avatar->nAvatars] = 0;
	}
	// now consider the other avatars
        for (int i=0; i < avatar->nAvatars; i++) {
          // if the other avatars are valid goals
          if (i != avatar->AvatarId && avatar->goals[i] == 1) {
  	    // then get their position in the maze structure
	    int otherAvaX = avatar->avatarsPos[i].x;
	    int otherAvaY = avatar->avatarsPos[i].y;

	    // calculate L1 distance
	    L1 = abs(otherAvaX - avatar->pos.x) + abs(otherAvaY - avatar->pos.y);
	    
	    // if this other avatar is in the same place as this thread's avatar 
	    // and the other avatar is a valid goal
	    if ((L1 == 0) && (avatar->goals[i] == 1)) {
		// then set it as an invalid goal
		avatar->goals[i] = 0;

		// if this thread's avatar has a higher id then the other avatar
		if (avatar->AvatarId > i) {
		  // then the other avatar becomes this avatar's leader
		  avatar->leader = i;
		  break;
		}
	    // otherwise insert the other avatar into the queue of possible goals
	    } else {
              priority_queue_insert(goals, get_node(avatar->maze, otherAvaX, otherAvaY), L1);  
	    } 
	  }
        }
	// if this thread's avatar does not have a leader 
	if (avatar->leader == -1) {
	  // then get its goal from the priority queue	
	  goal = priority_queue_extract(goals);
	}
      }
      // if this thread's avatar does have a leader
      if (avatar->leader != -1) {
	  // then get this leader's x and y coordinates
          int leaderAvaX = avatar->avatarsPos[avatar->leader].x;
          int leaderAvaY = avatar->avatarsPos[avatar->leader].y;

	  // and set it as this avatar's goal
	  goal = get_node(avatar->maze, leaderAvaX, leaderAvaY);

	  // if its leader and it are in the same place 
	  if ((leaderAvaX == avatar->pos.x) && (leaderAvaY == avatar->pos.y)) {
	    // then it does not move this turn
	    direction = M_NULL_MOVE;
	  }
      }
      if ((goal != NULL) && (direction == -1)) {
	// possible nodes to move to 
	priority_queue_t *maybeVisit = priority_queue_new();
	assertp(maybeVisit, "Failure to allocate memory for maybeVisit priority queue");
	
	// nodes to be visited during breath-first search
	priority_queue_t *toBeVisited = priority_queue_new();
	assertp(toBeVisited, "Failure to allocate memory for toBeVisited priority queue");
	
	// nodes already visited during breath-first search
	counters_t *visited = counters_new();
	assertp(visited, "Failure to allocate memory for visited hashtable");

	// get the current node this avatar is in 
	node_t *start = get_node(avatar->maze, avatar->pos.x, avatar->pos.y);
	// and add it to the visited list
	counters_add(visited, get_node_index(avatar->maze, start));
        	
	// loop through the directions from this avatar (0=west, 1=north, 2=east, 3=south)
	for (int i=0; i < 4; i++) {
	  // get the state of the direction (i.e. wall, unknown or connection)
	  int directionState = check_node_dir(avatar->maze, start, i);

	  // get the neighbor node in this direction
	  node_t *startNeighbor = get_neighbor(avatar->maze, start, i);

	  // if the direction is a wall or unknown
	  if(directionState == 3 || directionState == 1) {
             int unknowns=0;     // number of unknown directions seen along search
	     int priority=0;     // priority to insert items into the queue with so that it is FIFO

	     // insert this direction's node as the beginning node
	     priority_queue_insert(toBeVisited, startNeighbor, priority++);
	     
	     // the current node being visited
	     node_t *curr=NULL;

	     // as long as their are items to be visited then continue
	     while ((curr=priority_queue_extract(toBeVisited)) != NULL) {
	       // if the goal is found then set this avatar's direction 
	       // towards it
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
	       // loop through the directions of the current node
	       for (int j=0; j < 4; j++) {
		 // if an unknown path is found then increment counter for it
		 if (check_node_dir(avatar->maze, curr, j) == 1) {
		     unknowns++;
		 // otherwise if a connection is found
		 } else if (check_node_dir(avatar->maze, curr, j) == 3) {
		       // then get the neighbor their 
		       node_t *currNeighbor = get_neighbor(avatar->maze, curr, j);
	               
		       // added to the nodes to be visited if not already visited
		       if (counters_add(visited, get_node_index(avatar->maze, currNeighbor)) == 1) {
		         priority_queue_insert(toBeVisited, currNeighbor, priority++);
		       }
	         }
	       }
	     }
	     // the goal was found need to break again
	     if (direction != -1) {
	       break;
	     }
	     // if their were still unknowns along this direction and it is a connection
	     if (unknowns != 0 && directionState == 3) {
	       if ((avatar->trap > 0) && (pervSpot == startNeighbor)) {
		 avatar->trap--;
	         priority_queue_insert(maybeVisit, startNeighbor, get_L1_distance(startNeighbor, goal)+2000);
	       // if the avatar has backtracked a few times
	       } else if ((pervSpot == startNeighbor) && (avatar->back_tracked >= 3)) {
		 // then give its previous place the highest priority in the queue of possible destinations
                 priority_queue_insert(maybeVisit, startNeighbor, get_L1_distance(startNeighbor, goal)+2000);	
		 avatar->back_tracked=0;
		 avatar->trap=get_width(avatar->maze)/2;
	       // otherwise give the direction higher priority in the queue
	       } else {
                 priority_queue_insert(maybeVisit, startNeighbor, get_L1_distance(startNeighbor, goal)+1000);	       
	       }
	     } 
	  }
	  // if the direction is unknown insert it into the queue of possible destinations
	  if (directionState == 1) {
	    priority_queue_insert(maybeVisit, startNeighbor, get_L1_distance(startNeighbor, goal));
	  }
        }
	// if the direction has not been decided yet	
        if (direction == -1) {
          // extract a node from the queue of possible destinations
	  node_t *node = priority_queue_extract(maybeVisit); 

	  // if its the same as the pervious 
	  // one this avatar was at then take note of it
          if (pervSpot == node) {
	    avatar->back_tracked++;
          } else {
	    avatar->back_tracked = 0;
          }
	  // go in the direction of this node
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
	// free up memory
        priority_queue_delete(maybeVisit, NULL);	
	priority_queue_delete(toBeVisited, NULL);
	counters_delete(visited);

      // otherwise if this avatar still does not have a goal
      } else {
	  // then set direction to NULL Move
          direction = M_NULL_MOVE;
      }
      // free the queue of goals
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
          //checks if the move was successful 
          if (!error_msgs(move_resp)) {
            if (!end_program(move_resp) && !maze_solved(move_resp, avatar,fp)){
              if (ntohl(move_resp.type) == AM_AVATAR_TURN){   //gets the TurnID from the server and the XYPOS of each of the avatars   
                // remember the pervious place this avatar was at
		pervSpot = get_node(avatar->maze, avatar->pos.x, avatar->pos.y);

		// make the move, i.e. log its movement, update the avatar's variables
		// and update the maze
		make_move(avatar, move_resp, direction, avatar->maze, fp);

		maze_draw(avatar->maze);     // draw maze structure
	        num_of_moves++;		     // increment move counter
		location_writer(avatar, fp); // positions are logged 
                TurnID = ntohl(move_resp.avatar_turn.TurnId);  //the updated TurnID 
		fclose(fp);
		pthread_mutex_unlock(mutex1);
              }
            }
            else {
	      maze_draw(avatar->maze);       // draw maze structure
              printf("the game is over\n");  
	      printf("It took %d moves to finish the maze", num_of_moves);
              endgame = true;  		     // set endgame for other avatars
	      maze_delete(avatar->maze);     // free maze structure
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
  // free avatar struct's used memory
  free(avatar->avatarsPos);
  free(avatar->goals);
  free(avatar);
  return p; 
}

/* Helper function that prints any error messages in the case that the server returns an error
 * Input: 
 *    -resp: a message from the server
 * Output: 
 *    prints to stderr the error if the message from the server was an error
 */
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

/* Helper function that prints any error messages in the case that the server sends an error message
 * that would end the game.
 * Input: 
 *    -resp: a message from the server
 * Output: 
 *    prints to stderr the error if the message from the server was a game ending error
 */
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

/* Helper function that prints to the file the maze solved hash and to stdout that the maze was solved
 * Input:
 *    -resp: a message from the server
 *    -avatar: avatar to recieve message from server
 *    -fp: log file with avatars' actions
 * Output:
 *    prints to file and stdout
 */
static bool maze_solved(AM_Message resp, Avatar *avatar, FILE* fp)
{
  if(ntohl(resp.type) == AM_MAZE_SOLVED){	  
    printf("it's solved\n"); 
    fprintf(fp, "%d, %d, %d, %d\n", ntohl(resp.maze_solved.nAvatars), ntohl(resp.maze_solved.Difficulty), ntohl(resp.maze_solved.nMoves), ntohl(resp.maze_solved.Hash));
    return true; 
  }
  return false; 
}

/* Helper function that indicates whether the current game is over
 * Input:
 *    nothing
 * Output:
 *    bool indicating whether the current game is over
 */
static bool is_end_game(void)
{
  if (endgame == true){
    return true; 
  }
  else {
    return false; 
  }
}

/* Helper function that writes 
 * Input:
 *    -resp: a message from the server
 * Output:
 *    prints to stderr the error if the message from the server was an error
 */
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
  // loop through all the avatars and update their positions within the avatar struct accordingly
  // to the message given by the server
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
  // log what the avatar did in the log file
  if (direction == M_NULL_MOVE) {
    fprintf(fp, "Avatar %d stayed at %d,%d\n",avatar->AvatarId, avatar->pos.x, avatar->pos.y);
  } else if (tempX == avatar->pos.x && tempY == avatar->pos.y){
    fprintf(fp, "avatar %d ran into %s wall at %d,%d\n", avatar->AvatarId, wallDir, avatar->pos.x, avatar->pos.y);
    set_wall(get_node(maze, tempX, tempY), maze, dir);   // update maze structure
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
  // checking if avatar is at one of its goals and setting it to invalid if so
  if (avatar->pos.x == avatar->centerX && avatar->pos.y == avatar->centerY) {
    avatar->goals[avatar->nAvatars] = 0;
  }
  for (int i=0; i < avatar->nAvatars; i++) {
    if ((avatar->pos.x == avatar->avatarsPos[i].x) && (avatar->pos.y == avatar->avatarsPos[i].y)) {
      avatar->goals[i] = 0;
      if (avatar->AvatarId > i) {
        avatar->leader = i;
        break;
      }
    }
  }
}

