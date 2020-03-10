/* 
 * graphics.c - CS50 'graphics' module
 *
 * see graphics.h for more information.
 *
 * we_free, March 2020
 */

#include <stdio.h>
#include "maze.h"

/**************** local functions ****************/
/* not visible outside this file */
void direction_print(maze_t *maze, int x, int y, int dir);


/*********** global functions *************/
/**************** maze_draw() ****************/
void maze_draw(maze_t *maze) 
{
//  printf("******************************\n");

  int h = get_height(maze);
  int w = get_width(maze);

  // print the x axis of the maze with value of each column
  printf("   ");
  for (int i=0; i < w; i++) {
  	printf("%3d ", i);
  }

  printf("\n   ");
  for (int i=0; i < w; i++) {
  	direction_print(maze, i, 0, 1);
  }
  printf("+\n");

  // print the y axis with the value of each row
  for (int i=0; i < h; i++) {
     printf("%3d", i);
     direction_print(maze, 0, i, 0);
     for (int j=0; j < w; j++) {
	int avatar = get_avatar(get_node(maze, j, i));

	if (avatar == -1) {
	   printf("   ");
	} else {
	   printf(" %d ", avatar);
	}
	direction_print(maze, j, i, 2);
     }
     printf("\n   ");
     
     for (int j=0; j < w; j++) {
       direction_print(maze, j, i, 3);
     }
     printf("+\n");
  }
  //printf("******************************\n");
}

/************** global functions *************/

/**************** direction_print() ****************/
/* Purpose: 
 *  This helper function determines whether to draw
 *  a "?", "|", or " ", based on the server's response 
 *  to the avatars move. Uses check_noode_dir from  
 *  maze.h.
 * Input:
 *  A pointer to a maze struct
 *  Int's x, y, and dir (to represent directions)
 * Output:
 *  Nothing (void)
 */
void direction_print(maze_t *maze, int x, int y, int dir)
{
 node_t *node = get_node(maze, x, y);
 // east and west
 if (dir == 0 || dir == 2) {

   // pointing to unknown placeholder node
   if (check_node_dir(maze, node, dir) == 1) {
	printf("?");   
   } 
   // pointing to wall placeholder node
   else if (check_node_dir(maze, node, dir) == 2) {
      printf("|");
   }
   // connection to another node that we already discovered/inferred
   else if (check_node_dir(maze, node, dir) == 3) {
      printf(" ");
   }
 // north and south
 } else if (dir == 1 || dir == 3) {

    // pointing to unknown placeholder node
   if (check_node_dir(maze, node, dir) == 1) {
      printf("+ ? ");
   }
   // pointing to wall placeholder node 
   else if (check_node_dir(maze, node, dir) == 2) {
      printf("+---");
   } 
   // connection to another node that we already discovered/inferred
   else if (check_node_dir(maze, node, dir) == 3) {
      printf("+   ");
   }
 }
}
