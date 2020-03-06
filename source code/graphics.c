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


/** global functions **/
/**************** maze_draw() ****************/
void maze_draw(maze_t *maze) 
{
  printf("******************************\n");

  h = get_height(maze);
  w = get_width(maze);

  printf("   ");
  for (int i=0; i < w; i++) {
  	printf("%3d ", i);
  }

  printf("\n   ");
  for (int i=0; i < w; i++) {
  	direction_print(maze, i, 0, 1);
  }
  printf("+\n");

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
  printf("******************************\n");
}


void direction_print(maze_t *maze, int x, int y, int dir)
{
 node_t *node = get_node(maze, x, y);
 // east and west
 if (dir == 0 || dir == 2) {
   if (check_node_dir(maze, node, dir) == 1) {
	printf("?");   
   } else if (check_node_dir(maze, node, dir) == 2) {
      printf("|");
   } else if (check_node_dir(maze, node, dir) == 3) {
      printf("-");
   }
 // north and south
 } else if (dir == 1 || dir == 3) {
   if (check_node_dir(maze, node, dir) == 1) {
      printf("+ ? ");
   } else if (check_node_dir(maze, node, dir) == 2) {
      printf("+---");
   } else if (check_node_dir(maze, node, dir) == 3) {
      printf("+ | ");
   }
 }
}
