# Maze Project Design Spec

## Benjamin Matejka, Sebastian Saker, Christopher Sykes, Jacob Werzinsky, CS50, Winter 2020

## Overview


### User Interface 

##### In AMStartup.c:

The user interacts with the program on the command-line, it must always have three arguments:

`./AMStartup -n nAvatars -d Difficulty -h Hostname`

For example: 

`./AMStartup -n 3 -d 4 -h flume.cs.dartmouth.edu`

##### In amazing_client.c:

ASCII User Interface:
	A drawing of the maze structure is drawn at the end of each avatar’s move, where each node structure within the maze is represented by a drawing of each of its four cardinal directions. If the cardinal direction is in a state of ‘having a connection’ then a line is drawn from that node to its neighbor node in that direction, if the state is ‘no connection’ then a line is drawn separating the node and its neighbor node, and lastly if the state is ‘unknown’ then nothing is drawn between the node and its neighbor node. `AvatarId`s for each of the avatar threads is drawn into the node that the avatars occupy.

### Inputs and outputs

##### In AMStartup.c:

Inputs: the only inputs are command-line parameters; see the User Interface above.

Output(s): 
- one file to log the actions of all avatars, in the form of:
- The name will be in the form: Amazing_$USER_N_D.log, where $USER is the current user id, N is the value of nAvatars and D is the value of Difficulty.
- The first line of the file should contain $USER, the `MazePort`, and the date & time.

##### In amazing_client.c:

Inputs: 
- The program takes the following command line arguments, given by AMStartup.c:
  - `AvatarId`
  - `nAvatars`
  - `Difficulty`
  - Host name or IP address of the server
  - `MazePort`
- The log file listed in the outputs for AMStartup.c.
- The program takes input from the maze server in the form of the messages defined in [amazing.h](amazing.h).

Outputs:
- Each avatar will log their success/progress into the log file, including the `AM_MAZE_SOLVED` message .
- An ASCII drawn image of the maze is printed to standard output during each avatar’s turn.
- Messages to the server, examples of these can be found in [amazing.h](amazing.h).
		
### Functional decomposition into modules:

We anticipate the following modules or functions:

##### In AMStartup.c:
1. *main*, which validates arguments, establishes a connection with the server, creates a log file, and creates/initializes avatar threads.
##### In amazing_client.c:
2. *main*, which communicates with the server, sending and receiving messages.
3. *calculate_next_move*, which determines based on an avatar’s current position and heuristics which move, north, south, west or east, is best.
4. *maze* module, which defines the maze structure and its methods.
5. *priority_queue* module, which defines the priority queue structure and its methods.
6. *graphics* module, which contains all functions needed to display the ASCII UI.


### Major data structures

##### In AMStartup.c:
No major data structures.

##### In amazing_client.c:

The *maze* module provides two structures, the maze and the node…


*node*, a structure that represents a one by one square within the maze that will be solved. A *node* contains x and y coordinates representing its position on the maze grid and four cardinal direction variables that represent up to each of four neighbor maze squares. The direction variables will have four states: a state that indicates there is a connection to a node in that direction, a state that indicates there is no such connection i.e. there is a wall, a state that indicates the status of such a connection is unknown, and a state that indicates the connection is being explored by an avatar.


*maze*, a structure that represents a maze that will be solved. A *maze* is a collection of *node* structures, with the number of nodes being equal to the number of one by one spaces within a maze, each node being representative of a one by one space within a maze grid. *Nodes* within a *maze* structure will always be initialized with their respective x and y coordinates, and a status of unknown for their cardinal direction variables except in the case of the border *nodes*. The border *nodes*, those *nodes* representing spaces on the border of the maze grid, will have their cardinal direction variables that face outward from the center of the maze have a state indicating there is a wall. The unknown variables will have their statuses updated as the avatars navigate through the maze gathering more information.


The *priority_queue* module provides the *priority_queue* structure…

*priority_queue*, a structure that starts out empty and grows as (item, integer) pairs are added to it one at a time. Each item is given a priority in the queue based off of the passed integer, lower integers are given higher priority. Items are extracted, i.e. removed from the queue and returned to a caller, one at a time and the item with the current highest priority in the queue is always extracted. The item with the highest priority can be returned to a caller without removing it from the queue. Each item within the queue must be unique, and the priority for an item can be changed if needed.



### Pseudo code for logic/algorithmic flow

#### In AMStartup.c:
##### Main 
- Connect to the server
- Execute from the command line as show in the User Interface
- Parse the command line by their flags (-n/d/h) and store the information in variables
##### Validate the parameters (may call on another function to do this). 
    -n n Avatars: must be an integer, and less than a set value
    -d Difficulty: must be an integer greater than 0 and less than 10.
    -h Hostname: must be (char *) to flume.cs.dartmouth.edu
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
- Create N copies of the avatar client (as threads), with the parameters that they need:
  - `AvatarId`
  - `nAvatars`
  - `Difficulty`
  - Host name or IP address of the server
  - `MazePort`
  - Filename of the log the avatar should open for writing in append mode
  - NOTE: The client program is not really meant to be run by people, so the parameters can be simply positional and required.

#### In amazing_client.c:
- Extract the maze port, maze height, and maze width from `MazePort` using `ntohl()`
- If `AvatarId` equals 0 then initialize a new *maze* structure using the maze height and maze width. 
- Attempt to connect to the maze server through the maze port. 
  - If failure, kill the amazing_client and all its processes.
  - If successful, send an `AM_AVATAR_READY` message to the server containing this thread’s `AvatarId` using `htonl()`.
- Continue in a loop:
  - Wait for a message from the server.
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
      - Within the maze structure, update the directions that have states of ‘being explored by an avatar’ to states of ‘no connection/wall’.
      - Within the maze structure, update all directions that must exist given the new state of `no connection/wall` and the fact all places in the maze are accessible.
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
  - Else set next move the avatar will make to M_NULL_MOVE.
  - Encode the next move and `AvatarId` into `AM_AVATAR_MOVE` using `htonl()` and send this message to the server.
- If any of the exit conditions occur, do any necessary clean up: close the log file and free any memory used by the maze structure and close sockets. 

### Dataflow through modules
#### In AMStartup.c:
- Only one function, Main, which validates arguments, establishes a connection with the server, creates a log file, and creates/initializes avatar threads. 
#### In amazing_client.c:
- Main will take arguments from AM_STARTUP and initialize the maze structure through calling an appropriate function from the *maze* module. It will then wait for a message from the server.
- If the message received is AM_AVATAR_TURN then *main* calls methods from the *maze* module which will handle all logic that pertains to updating nodes within the maze structure and guessing which directions must exist. The *maze* functions will return control to *main*
- *main* will call functions from the *graphics* module, two to be exact. One will draw to standard output the current maze structure and another will draw the avatars within that maze structure.
- *determine_goal* will calculate the most optimal move for each avatar according to the A-star-inspired priorityqueue.
- *calculate_next_move*
- If *main* receives any error messages or `AM_MAZE_SOLVED`, it will do any necessary clean up to free up memory used by the priority queues and maze structure, and close the log file.

### Testing Plan
Unit testing
#### Server side:
1. Test to see if reading and validating command line correctly by inputting edge cases
2. Test to see if able to connect to the server and receive the appropriate message
3. Test to see if able to create log file of the proper format and name
4. Test to see if able to create avatar threads

##### Client(avatar) side:
1. Test to see if we can calculate the average distance between 4 avatars within a given maze, then check to see if our goal coordinate is correct.
2. Test to see if the A-star-inspired search works by creating a game with 2 avatars, then have one of those avatars remain in place in order to act as the goal for the other avatar which will move. That avatar should move according to an A-star search. In order to validate this, we will display the avatars path to see if it is moving correctly.
3. Test to see if our priority queue works. 


##### Integration Testing
- Test if information between server and client in creating maze is successful
- Also test to see if the maze is able to be solved
