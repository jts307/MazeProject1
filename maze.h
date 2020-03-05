/* 
 * maze.h - header file for CS50 maze module
 * 
 * Sebastian Saker, March 2020
 */

#ifndef __MAZE_H
#define __MAZE_H

#include <stdio.h>
#include <stdbool.h>

/**************** global types ****************/
typedef struct maze maze_t; // opaque to users of the module
typedef struct node node_t;
/**************** functions ****************/

/**************** FUNCTION ****************/
/* Create a new maze structure.
 *
 * We return:
 *   pointer to a new maze; NULL if error. 
 * Caller is responsible for:
 *   later calling maze_delete();
 */
maze_t *maze_new(const int height, const int width);

/**************** check_node ****************/
int check_node(node_t *node); //return an int based on what the node is pointing to
//return 0 if NULL
//return 1 if node is pointing to unknown placeholder node
//return 2 if node is pointing to wall placeholder node
//return 3 if node has a connection to another node that we already discovered/inferred

/**************** set_wall ****************/
void set_wall(node_t *node, maze_t *maze, const int dir); //dir will indicate which direction we want to place a wall

/**************** set_connection ****************/
void set_connection(node_t *node, maze_t *maze, const int dir); //dir will indicate which direction we want to me a connection                                                                /**************** counters_set ****************/

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

#endif // __MAZE_H
