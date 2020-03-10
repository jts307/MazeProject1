/* 
 * graphics.h - header file for CS50 graphics module
 * 
 * The graphics module prints the ASCII representation
 *  of the maze and avatars. 
 *  
 * We_free, March 2020
 */

#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdio.h>
#include <stdbool.h>

/**************** maze_draw() ****************/
/* Purpose: 
 *  maze_draw takes a pointer to a maze struct
 *  containing the MazeHeight and MazeWidth, and 
 *  draws the ASCII representation of the maze and
 *  avatars to the stdout
 * 
 * Input:
 *  A pointer to a maze struct
 * 
 * Output:
 *  An ASCII representation of the maze and avatars
 *  printed to the stdout
 */
void maze_draw(maze_t *maze);

#endif // __GRAPHICS_H
