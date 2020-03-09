/* 
 * maze.h - header file for CS50 maze module
 * 
 * we_free, March 2020
 */

//incorporate if I know other walls/connections because of wall/connection
//incorporte the loop mechanic where no nodes can create loops

#ifndef __MAZE_H
#define __MAZE_H

#include <stdio.h>
#include <stdbool.h>
#include "./libcs50/counters.h"

/**************** global types ****************/
typedef struct maze maze_t; // opaque to users of the module
typedef struct node node_t;
/**************** functions ****************/

/**************** maze_new ****************/
/* Create a new maze structure.
 *
 * We return:
 *   pointer to a new maze; NULL if error. 
 * Caller is responsible for:
 *   later calling maze_delete();
 */
maze_t *maze_new(const int height, const int width);

/**************** get_node ****************/
/* Get a node from the maze structure.
 * We need:
 *   -maze: the maze struct to get a node from
 *   -x: x coordinate of the node 
 *   -y: y coordinate of the node
 * We return:
 *   pointer to the specified node, NULL on any error
 */
node_t *get_node(maze_t *maze, int x, int y);

/**************** get_index ****************/
//Translates x and y coordinates to the index number of the maze_nodes stored in the maze
//returns the index number of where these x and y coordinates's node is stored in the array of nodes
//else return -1 if node does not exist in maze_nodes
int get_index(maze_t *maze, int x, int y);

/**************** get_avatar ****************/
//returns an integer that represents the avatar's ID of the avatar currently on the node
//if there is no avatar, return -1
int get_avatar(node_t *node);

/**************** set_avatar ****************/
//set an integer on a node that represents the avatar's ID of the avatar currently on the node
void set_avatar(node_t *node, int avatar);

/**************** get_height ****************/
//returns an integer that represents the height of the maze
int get_height(maze_t *maze);

/**************** get_width ****************/
//returns an integer that represents the width of the maze
int get_width(maze_t *maze);

/**************** get_node_x ****************/
//returns an integer that represents the X coordinate of a node
int get_node_x(node_t *node);

/**************** get_node_y ****************/
//returns an integer that represents the Y coordinate of a node
int get_node_y(node_t *node);

/**************** check_node ****************/
int check_node_dir(maze_t *maze, node_t *node, int dir); //return an int based on what the node is pointing to
//return 0 if NULL
//return 1 if node is pointing to unknown placeholder node
//return 2 if node is pointing to wall placeholder node
//return 3 if node has a connection to another node that we already discovered/inferred

/**************** further_deduce_connection ****************/
//recursively checks if there is a connection that can be deduced if
//we know there are was a dead end previously, and therefore the following nodes cannot be dead ends
//until we reach a node with less than 2 walls
void further_deduce_connection(node_t *node, maze_t *maze);

/**************** deduce_connection ****************/
//checks if there is a connection that can be deduced if
//we know there are three walls for a given node (a deadend)
void deduce_connection(node_t *node, maze_t *maze);

//helper function for check_loop
void check_loopR(node_t *node, maze_t *maze, counters_t *check_for_loop, const int dir);

//determine whether or not there is a loop created as a result of assuming our unknown to be a connection
bool check_loop(node_t *node, maze_t *maze, counters_t *nodes_so_far, const int dir);

/**************** deduce_wallR ****************/
//recursively checks if there is a wall that can be deduced if
//we know a loop would exist if a particular node has a connection, which is impossible
//since our maze has no loops!
//use a dfs
void deduce_wallR(node_t *node, maze_t *maze, counters_t *nodes_so_far, const int dir);

/**************** deduce_wall ****************/
//recursively checks if there is a wall that can be deduced if
//we know a loop would exist if a particular node has a connection, which is impossible
//since our maze has no loops!
//use a dfs
void deduce_wall(node_t *node, maze_t *maze);

/**************** set_wall ****************/
void set_wall(node_t *node, maze_t *maze, const int dir); //dir will indicate which direction we want to place a wall

/**************** try_connection ****************/
//same as set_connection, but does not assume the connection is true
void try_connection(node_t *node, maze_t *maze, const int dir); //dir will indicate which direction we want to make a connection

/**************** set_connection ****************/
void set_connection(node_t *node, maze_t *maze, const int dir); //dir will indicate which direction we want to make a connection

/**************** set_unknown ****************/
void set_unknown(node_t *node, maze_t *maze, const int dir); //dir will indicate which direction we want to reset a connection to unknown

/**************** maze_delete ****************/
/* Delete the whole maze.
 *
 * Caller provides:
 *   a valid pointer to maze.
 * We do:
 *   we ignore NULL ctrs.
 *   we free all memory we allocate for this maze.
 */
void maze_delete(maze_t *maze);

//helper function for inserting node's indexes of nodes seen so far into new counterset
void insert_node(void *arg, const int key, const int count);
//helper for detecting if there is a loop
void isloop(void *arg, const int key, const int count);

#endif // __MAZE_H
