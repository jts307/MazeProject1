# project_maze_challenge
# CS50 Winter 2020, Final Project

## Team name: We_free
## Team Members: Jacob Werzinsky, Sebastian Saker, Chris Sykes, Ben Matejka

GitHub usernames: jts307, chriscsykes, benmatejka, ssaker18

### Overview

The overall goals of the project are to design, implement and test a client application that simulates players --avatars-- searching for each other in a maze generated and managed by a server. The application should be able to solve mazes of varying difficulty without any prior knowledge. In addition, the application should create a visual user interface and a log file to provide the avatars’ success/failures. 

### High Level Description of Algorithm for Solving Mazes

The algorithm begins by assigning each avatar a set of goals, which are initially the other avatars in the maze and the center point between all avatars in the maze. On any given turn, an avatar will first begin by updating all its goals. A goal will become invalid when an avatar reaches it and not be considered as a goal. The avatar will then take the nearest goal out of the list of goals it has availible and set that as its current goal. Then all paths, i.e. north, south, east and west, that an avatar can take are considered by the avatar, if a path is known to be a wall the avatar will not consider it. The algorithm then looks down paths that are known and checks for deadends through a breath-first search. If while doing this breath first search the goal of an avatar is found then the avatar is set in the direction of this path. If the path turns out to be a deadend the algorithm ignores this path. If it is not a deadend this path is inserted into a priority queue of potential moves with a priority equal to its L1 distance plus a huge number (Note: the priority queue it set up so that lower priority things are first in a queue). Then paths that are unknown are considered. Each unknown path is inserted into the queue with a priority equal to their L1 distance. The path with the lowest priority is taken from the queue and the avatar travels along that path. If an avatar runs into another avatar, the avatar with a lower AvatarId becomes a leader. The non-leader avatar follows the leader as it searches for other goals. They continue like this until they run out of goals, at which point they have found each other.

### Directory Structure
The repository is organized as follows:

- results
    - Amazing_$USER_N_D.log
- source code
    - libcs50
        - memory.c
        - memory.h
    - AMStartup.c
    - amazing.h
    - avatar.c
    - avatar.h
    - maze.c
    - maze.h
    - priority_queue.c
    - priority_queue.h
- .gitignore
- DESIGN.md
- README.md
- Makefile

### Compiling
To compile, cd into the source code directory, type `make` into the terminal and press enter.

To clean the directory, type `make clean` into the terminal and press enter.

### Execution
To run the program:
`./AMStartup.c -n nAvatars -d Difficulty -h Hostname`

- nAvatars must be an integer 1-10 inclusive
- Difficulty must be an integer 0-9 inclusive
- Hostname must be flume.cs.dartmouth.edu  

### Assumptions
- If an invalid option or argument is passed, the program will exit and print an error message. It will not continue witha a default value. 
- The program assumes that flume.cs.dartmouth.edu will send it the correct information to start the game. 
- Logfiles are created in the format Amazing_$USER_N_D.log, where $USER is the current userid, N is the value of nAvatars and D is the value of Difficulty. If a logfile already exists with this name, that file will be overwritten with the most current run.
