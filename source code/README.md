# source code directory
# CS50 Winter 2020, Final Project

## Team name: We_free
## Team Members: Jacob Werzinsky, Sebastian Saker, Chris Sykes, Ben Matejka

### Directory
The following is included in the `source code` directory:
- libcs50 directory, which includes memory.c/h
- AMStartup.c
- README.md
- amazing.h
- avatar.c/h
- maze.c/h
- priority_queue.c/h

### Implementation Strategy
*On piazza, prof said to put the heuristic/method for solving the maze*

#### AMStartup.c
Purpose: 
- AMStartup.c contains the `main` method that validates arguments, establishes a connection with the server, creates a log file, and creates/initializes avatar threads. Each of these threads will run the `avatar_play` method in `avatar.h`. It also closes threads and frees memory when the maze has been solved.  

Input:
- This function takes its input from the command line. See *compilation and execution* for more info

Output:
- A log file containg info about the outcome of the avatars in the maze.

Possible Failures:
- Not aware of any. Program will exit if invalid parameter is passed.

### Compilation and Execution
To compile, cd into the source code directory, type `make` into the terminal and press enter.

To run the program:
`./AMStartup.c -n nAvatars -d Difficulty -h Hostname`

- nAvatars must be an integer 1-10 inclusive
- Difficulty must be an integer 0-9 inclusive
- Hostname must be flume.cs.dartmouth.edu 

To clean the directory, type `make clean` into the terminal and press enter.