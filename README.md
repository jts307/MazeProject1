# project_maze_challenge
# CS50 Winter 2020, Final Project

## Team name: We_free
## Team Members: Jacob Werzinsky, Sebastian Saker, Chris Sykes, Ben Matejka

GitHub usernames: jts307, chriscsykes, benmatejka, ssaker18

### Overview

The overall goals of the project are to design, implement and test a client application that simulates players --avatars-- searching for each other in a maze generated and managed by a server. The application should be able to solve mazes of varying difficulty without any prior knowledge. In addition, the application should create a visual user interface and a log file to provide the avatars’ success/failures. 

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