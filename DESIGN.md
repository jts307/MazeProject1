# Maze Project Design Spec

## Benjamin Matejka, Sebastian Saker, Christopher Sykes, Jacob Werzinsky
## CS50, Winter 2020

### Overview

The overall goals of the project are to design, implement and test a client application that simulates players --avatars-- searching for each other in a maze generated and managed by a server. The application should be able to solve mazes of varying difficulty without any prior knowledge. In addition, the application should create a visual user interface and a log file to provide the avatars’ success/failures. 

### User Interface 

##### In AMStartup.c:

The user interacts with the program on the command-line, it must always have three arguments:

`./AMStartup -n nAvatars -d Difficulty -h Hostname`

For example: 

`./AMStartup -n 3 -d 4 -h flume.cs.dartmouth.edu`

##### In avatar.c:

ASCII User Interface:
	A drawing of the maze structure is drawn at the end of each avatar’s move, where each node structure within the maze is represented by a drawing of each of its four cardinal directions. If the cardinal direction is in a state of ‘having a connection’ then a line is drawn from that node to its neighbor node in that direction, if the state is ‘no connection’ then a line is drawn separating the node and its neighbor node, and lastly if the state is ‘unknown’ then nothing is drawn between the node and its neighbor node. `AvatarId`s for each of the avatar threads is drawn into the node that the avatars occupy.

### Inputs and outputs

##### In AMStartup.c:

Inputs:
- Input in the form of the command-line parameters; see the User Interface above.
- Messages from the server including `AM_INIT_OK`, `AM_INIT_FAILED`, etc. These can be found in [amazing.h](amazing.h).

Output(s): 
- one file to log the actions of all avatars, in the form of:
- The name will be in the form: Amazing_$USER_N_D.log, where $USER is the current user id, N is the value of nAvatars and D is the value of Difficulty.
- The first line of the file should contain $USER, the `MazePort`, and the date & time.
- Messages to the server: `AM_INIT` which can be found in [amazing.h](amazing.h).

##### In avatar.c:

Inputs: 
- No inputs directly from the user 
- The program takes input from the maze server in the form of the messages defined in [amazing.h](amazing.h). These include `AM_MAZE_SOLVED`, `AM_UNKNOWN_MSG_TYPE`, `AM_UNEXPECTED_MSG_TYPE`, `AM_AVATAR_OUT_OF_TURN`, `AM_TOO_MANY_MOVES`, `AM_SERVER_TIMEOUT`, `AM_SERVER_DISK_QUOTA`, and `AM_SERVER_OUT_OF_MEM`.

Outputs:
- Each avatar will log their success/progress into the log file, including the `AM_MAZE_SOLVED` message.
- An ASCII drawn image of the maze is printed to standard output during each avatar’s turn.
- Messages to the server, these can be found in [amazing.h](amazing.h). These include `AM_AVATAR_READY` and `AM_AVATAR_MOVE`.
		
### Functional decomposition into modules:

We anticipate the following modules or functions:

##### In AMStartup.c:
1. *main*, which validates arguments, establishes a connection with the server, creates a log file, and creates/initializes avatar threads.
##### In avatar.c:
2. *avatar_play*, which communicates with the server, sending and receiving messages, initializes data structures and writes to the log file. It also determines each avatar's move, checks when the game is over and frees allocated memory and closes the socket when finished. *avatar_play* is concurrently run by every thread. 
3. *determine_goal*, which determines the goal for an avatar at the beginning of its turn.
4. *calculate_next_move*, which determines, based on an avatar’s current position and heuristics, which move, north, south, west or east, is best.
5. *maze* module, which defines the maze structure and its methods.
6. *priority_queue* module, which defines the priority queue structure and its methods.
7. *graphics* module, which contains all functions needed to display the ASCII UI.


### Major data structures

##### In AMStartup.c:
No major data structures.

##### In avatar.c:

The *maze* module provides two structures, the maze and the node…


*node*, a structure that represents a one by one square within the maze that will be solved. A *node* contains x and y coordinates representing its position on the maze grid and four cardinal direction variables that represent up to each of four neighbor maze squares. The direction variables will have four states: a state that indicates there is a connection to a node in that direction, a state that indicates there is no such connection i.e. there is a wall, a state that indicates the status of such a connection is unknown, and a state that indicates the connection is being explored by an avatar.

*maze*, a structure that represents a maze that will be solved. A *maze* is a collection of *node* structures, with the number of nodes being equal to the number of one by one spaces within a maze, each node being representative of a one by one space within a maze grid. *Nodes* within a *maze* structure will always be initialized with their respective x and y coordinates, and a status of unknown for their cardinal direction variables except in the case of the border *nodes*. The border *nodes*, those *nodes* representing spaces on the border of the maze grid, will have their cardinal direction variables that face outward from the center of the maze have a state indicating there is a wall. The unknown variables will have their statuses updated as the avatars navigate through the maze gathering more information. The maze structure will always form a tree, so if any predictions as to whether or not a connection must exist can be made the maze structure will make them.

The *priority_queue* module provides the *priority_queue* structure…

*priority_queue*, a structure that starts out empty and grows as (item, integer) pairs are added to it one at a time. Each item is given a priority in the queue based off of the passed integer, lower integers are given higher priority. Items are extracted, i.e. removed from the queue and returned to a caller, one at a time and the item with the current highest priority in the queue is always extracted. The item with the highest priority can be returned to a caller without removing it from the queue. Each item within the queue must be unique, and the priority for an item can be changed if needed.

### Pseudo code for logic/algorithmic flow

#### In AMStartup.c:
- Connect to the server
- Execute from the command line as show in the User Interface
- Parse the command line by their flags (-n/d/h) and store the information in variables
- Validate the parameters (may call on another function to do this). 
    - -n n Avatars: must be an integer, and less than a set value
    - -d Difficulty: must be an integer greater than 0 and less than 10
    - -h Hostname: must be (char *) to flume.cs.dartmouth.edu
- Send the `AM_INIT` message to the server at the `AM_SERVER_PORT`
    - Use `htonl()` to construct a message in mask the bits of the message in a way that the server can read them. In the message, specify:
      - The desired number of avatars
      - The difficulty of the maze
- Wait for the server to respond with `AM_INIT_OK` message.
  - Extract the message using `ntohl()`
    - If the server responds with `AM_INIT_OK`
       - Recover the `MazePort`, as well as the `MazeHeight` and `MazeWidth` (ints)
    - If the server responds with `AM_INIT_FAILED`
       - Return error. This can happen if `nAvatars` > `AM_MAX_AVATAR` or `Difficulty` > `AM_MAX_DIFFICULTY`
- Create a new log file in the form Amazing_$USER_N_D.log
  - where $USER is the current user id
  - N is the value of nAvatars and
  - D is the value of Difficulty.
  - The first line of the file should contain $USER, the `MazePort`, and the date & time.
- Create N copies of the avatar client (as threads) using *avatar_new*, with the parameters that they need:
  - `AvatarId`
  - `nAvatars`
  - `Difficulty`
  - Host name or IP address of the server
  - `MazePort`
  - Filename of the log the avatar should open for writing in append mode
  - NOTE: The client program is not really meant to be run by people, so the parameters can be simply positional and required.
  - Each thread runs *avatar_play* when it's created 

#### In avatar.c:
- initialize an Avatar struct from the parameter
- initialize: the mutex1 lock, the number of moves made, the previous position of the avatar, how many times an avatar          repeated a move. 
- allocate space for `avatarsPos` and `goals` 
- give each Avatar a valid goal   
- Attempt to connect to the maze server through the maze port. 
  - If failure, kill the avatar_play and all its processes.
  - If successful, send an `AM_AVATAR_READY` message to the server containing this thread’s `AvatarId` using `htonl()`.
- Continue in a loop while the game is not over using the helper function `is_end_game(void)` 
  - Wait for a message from the server.
    - using `static bool error_msgs(AM_Message resp)` and `static bool end_program(AM_Message resp)`: 
      - If the message is `AM_AVATAR_TURN`, extract the `TurnID` and Avatar Positions using `nothl()`
      - If the message is `AM_TOO_MANY_MOVES`, print a losing message, and break from the loop
      - If the message is `AM_SERVER_TIMEOUT`, print an error message, and break from the loop
      - If the message is `AM_MAZE_SOLVED`, then…
        - If this thread’s `AvatarId` equals 0 then write the `AM_MAZE_SOLVED` message to the log file after decoding it using `ntohl()`.
        - Break from the loop
      - If the thread’s connection to the server is broken, print an error message, and break the loop.
      - If `AM_SERVER_DISK_QUOTA` and `AM_SERVER_OUT_OF_MEM` messages are read then print an error message, and break the loop.
    
  - If the priority queue containing possible avatar goals for this thread’s avatar does not exist then…
    - Write to the log file indicating that this avatar was inserted at its current position.
    - Initialize a priority queue for the avatar’s possible goals. 
    - Calculate the average of all the avatars’ (x,y) positions to find an approximate center point.
    - Insert the *nodes* associated with the center point, and each of the other avatars’ positions into the priority queue with an integer value equal to each node’s L1 distance from this thread’s avatar node.
    - View a node from the priority queue and set this as this avatar’s goal.
    - Set this avatar’s leader status to true.
  - If `TurnID` equals this thread’s `AvatarId` then…
    - Update all avatar positions in the destinations priority queue according to the Avatar Positions received.
    - If the last avatar that moved position was not updated then...
      - Within the maze structure, update the directions that have states of ‘being explored by an avatar’ to states of ‘no connection’.
      - Within the maze structure, update all directions that must exist given the new state of 'no connection' and the fact all places in the maze are accessible.
      - Write to the log file indicating that the last avatar ran into a wall.
    - If the last avatar that moved position was updated then...
      - Within the maze structure, update the directions that have states of ‘being explored by an avatar’ to states of ‘there is a connection’.
      - Write to the log file indicating that the last avatar who moved location.
    - Draw the updated maze structure to the standard output by looping through each node within the maze structure and calling an appropriate function from the *graphics* module on each node.
    - Draw the avatars with their current positions into the appropriate place within the drawn maze by calling an appropriate function from the *graphics* module. 
    - If the avatar is already at one or more of its goals and has a true leader status then...
      - Extract the goal(s) from the priority queue that is storing avatar goals
      - If one of the goals was another avatar and that other avatar had a higher `AvatarId` then this one, set its goal to be that other avatar and update its leader status to false.
    - If the avatar has a true leader status then…
      - Set this avatar’s goal to the node with highest priority in the goals priority queue, without removing it from the queue.
      - If there are no goals remaining in the queue, then the avatar should not have a goal.
    - If the avatar has a goal and this goal is in a different place than the avatar then…
      - If the priority queue containing to be visited nodes does not exist then create one.
      - Make sure the priority queue is empty.
      - Then add all the neighbors of the avatar’s node to the to be visited priority queue with an integer value equal to their L1 distance from the avatar’s current goal.
        - if not the first move or if the goals have not changed since the last move, do not insert the node saved from the last move.
      - Loop through this avatar’s node’s directions in the maze structure...
        - If a direction has a connection then perform a depth-first search on that direction, using a priority queue as the stack (give higher priority to those added later on).
          - If the goal is found then set the next move to this direction
          - If there are no unknown directions explored during the search and the goal was not found then remove this direction’s node from the to be visited priority queue.
      - If the next move was not determined then…
        - Extract a node from the to be visited priority queue.
        - Save node.
        - Set next move to the direction that this node is in.
    - Else set next move the avatar will make to `M_NULL_MOVE` i.e. it will not make a move.
    - Encode the next move and `AvatarId` into `AM_AVATAR_MOVE` using `htonl()` and send this message to the server.
- If any of the exit conditions occur, do any necessary clean up: close the log file and free any memory used by the *maze/priority queue* structures and close sockets. 

### Dataflow through modules
#### In AMStartup.c:
- Only one function, *main*, which validates arguments, establishes a connection with the server, creates a log file, and creates/initializes avatar threads.  
#### In avatar.c:
1. *Avatar_play* will take the Avatar struct argument from *AM_Startup.c*. It will then wait for a message from the server.
2. If the message received is `AM_AVATAR_TURN` then *main* calls methods from the *maze* module which will handle all logic that pertains to updating nodes within the maze structure and guessing which directions must exist. The *maze* functions will return control to *main*.
3. *avatar_play* will write any updates of the avatars’ positions to the log file.
4. *avatar_play* will call functions from the *graphics* module, two to be exact. One will draw to standard output the current maze structure and another will draw the avatars within that maze structure. They both will return control to *main*.
5. *main* then calls the *determine_goal* function which will find the closest node out of the other avatars and center point, and set that as an avatar’s goal node. After this, the function returns control to *main*.
6. *main* then calls *calculate_next_move* which will calculate an avatar’s next ‘best’ move given an avatar, its goal node and the maze structure. This returns control to *main*, which sends this best move to the server.
7. If *avatar_play* receives any error messages or `AM_MAZE_SOLVED`, it will do any necessary clean up to free up memory used by the priority queues and maze structures, and close the log file. 

### Testing Plan
#### *Unit Testing*
#### In AM_Startup.c:
1. Test to see if reading and validating command line correctly by inputting a variety of incorrect parameters.
2. Test to see if the program is able to connect to the server and receive the appropriate messages, by printing them to the standard output as they are received and sent.
3. Test to see if the program is able to create a log file of the proper format and name by doing a run of the program and killing it just after it starts up the avatar threads. Then manually checking if the log file is in the proper format.
4. Test to see if the program is able to create multiple avatar threads by calling print statements within the avatar threads and counting how many print statements occured and see if they meet expectations given any amount of threads we might create. Ideally, this would be done with the code to communicate with the server, calculate the next move, etc. within the `avatar_client.c` commented out.
#### In avatar.c:
1. Test to see if *avatar_play* is sending and receiving the proper messages by printing the messages to standard output right before they get sent or right after they are received. 
2. Test the *maze* module by manually creating a maze structure and using all its methods, and printing it out in a human friendly format. Then seeing if the printed out maze structures meet expectations. 
3. Do the same thing for the *priority_queue* module, i.e. use all its methods and print out a queue in a human friendly format to make sure all its methods and such work.
4. Run the program on the level 0 maze with 2 avatars while printing to standard output the current positions of the avatars each time they are received. Compare this to what the ASCII UI is outputting and what is in the log file to see if these are displaying information properly, i.e. test the *graphics* module.
5. Test to see if we can calculate the average distance between 4 avatars within a given maze, then check to see if our goal coordinate is correct in all situations, i.e. test if *determine_goal* works.
6. Test to see if the move calculation algorithm works by creating a game with 2 avatars on the level 0 maze. Run through the algorithm by hand and see if it follows exactly how the program runs it. Determine if the algorithm succeeded or failed the maze game, i.e. test the *calculate_next_move* function.
7. Do the same test as test 2 but with 3 avatars instead.
8. Do the same test as test 2 but on the level 1 maze. 
#### *Integration Testing (includes both AM_Startup.c and avatar.c)*
1. Since avatars spawn in the mazes randomly, to get an idea of whether or not our program works we must do several tests on each difficulty level. As a completely random guess, we will say that if our program makes it successfully through a maze at difficulty n 30 times without any errors and with the `AM_MAZED_SOLVED` message written to the log file each time then it works at that difficulty. We will do these tests starting at difficulty 0 with 0-4 avatars and work our way up to each progressively harder difficulty.

