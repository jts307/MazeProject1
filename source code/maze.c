/* 
 * maze.c - CS50 'maze' module
 *
 * see maze.h for more information.
 *
 * we_free, March 2020
 */

#include <stdio.h>
#include <stdbool.h>
#include "maze.h"
#include "memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct node
{
    int x, y; // the x and y coordinates of the maze node
    struct node *n; // pointer to mazenode's north node
    struct node *s; // pointer to mazenode's south node
    struct node *e; // pointer to mazenode's east node
    struct node *w; // pointer to mazenode's west node
    int avatar;     // what avatar is here, -1 if none
} node_t;

/**************** global types ****************/
typedef struct maze
{
    struct node *unknown;     //pointer for representing node's that have not been discovered yet
    struct node *wall;        //pointer for representing walls between nodes
    struct node **maze_nodes; // array of mazenodes for the maze discovered/seen/deduced so far
    int H, W;           // height and width of maze
} maze_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see maze.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static node_t *node_new(maze_t *maze, const int x, const int y, const int border); //use border int to determine where the wall should be given a border node

/**************** maze_new() ****************/

maze_t *maze_new(const int height, const int width)
{
    maze_t *maze = count_malloc(sizeof(maze_t));

    if (maze == NULL)
    {
        return NULL; // error allocating counters
    }
    else
    {
        // initialize contents of maze structure
        //wall and unknown placeholder nodes shall have impossible coordinates
        maze->wall = node_new(maze, -1, 0, 0); 	//initialize placeholder node for representing walls between nodes
        node_t *wall = maze->wall;
	maze->unknown = node_new(maze, 0, -1, 0);                                        //initialize placeholder node for representing directions that have not been discovered/are unknown
	maze->H = height;                                                       //height of maze
        maze->W = width;                                                        //width of maze
        maze->maze_nodes = count_malloc((height * width) * (sizeof(node_t))); //check with jacob if it is sizeof pointer or just sizeof node
        node_t **mazeNodes = maze->maze_nodes;
	int numOfNodes = height * width;
        int x;
        int y;
        for (int i = 0; i < numOfNodes; i++) //initialize mazeNodes
        {                                    //the nodes will be stored from bottom to top, left to right
            x = i % width;                   //x coordinate
            y = i / width;                   //y coordinate
            if (x == 0 && y == height - 1)
            {
                mazeNodes[i] = node_new(maze, x, y, 3); //south-west border node
                mazeNodes[i]->w = wall;
                continue;
            }
            if (x == width - 1 && y == height - 1)
            {
                mazeNodes[i] = node_new(maze, x, y, 3); //south-east border node
                mazeNodes[i]->e = wall;
                continue;
            }
            if (x == 0 && y == 0)
            {
                mazeNodes[i] = node_new(maze, x, y, 1); //north-west border node
                mazeNodes[i]->w = wall;
                continue;
            }
            if (x == width - 1 && y == 0)
            {
                mazeNodes[i] = node_new(maze, x, y, 1); //north-east border node
                mazeNodes[i]->e = wall;
                continue;
            }
            if (x == 0)
            {
                mazeNodes[i] = node_new(maze, x, y, 4); //west border node
                continue;
            }
            if (x == width - 1)
            {
                mazeNodes[i] = node_new(maze, x, y, 2); //east border node
                continue;
            }
            if (y == height - 1)
            {
                mazeNodes[i] = node_new(maze, x, y, 3); //south border node
                continue;
            }
            if (y == 0)
            {
                mazeNodes[i] = node_new(maze, x, y, 1); //north border node
                continue;
            }
            mazeNodes[i] = node_new(maze, x, y, 0); //node is not surrounded by border
        }
        return maze;
    }
}

/**************** node_new ****************/
/* Allocate and initialize a mazenode */
// the 'static' modifier means this function is not visible
// outside this file
static node_t *node_new(maze_t *maze, const int x, const int y, const int border)
{
    node_t *node = count_malloc(sizeof(node_t));

    if (node == NULL)
    {
        // error allocating memory for node; return error
        return NULL;
    }
    else
    {
        node->x = x;       //node's y coordinate
        node->y = y;       //node's y coordinate
	node->avatar = -1;
        node->n = maze->unknown; //default will point to unknown
        node->s = maze->unknown;
        node->w = maze->unknown;
        node->e = maze->unknown;
        if (border == 1) //node is on north border, therefore we know a wall is already there
        {
            node->n = maze->wall;
        }
        if (border == 2) //node is on east border, therefore we know a wall is already there
        {
            node->e = maze->wall;
        }
        if (border == 3) //node is on south border, therefore we know a wall is already there
        {
            node->s = maze->wall;
        }
        if (border == 4) //node is on west border, therefore we know a wall is already there
        {
            node->w = maze->wall;
        }
        if (border == 0) //node is not on a border, therefore we do not know where any walls are
        {                //do nothing
        }
        return node;
    }
}

/**************** get_node ****************/
node_t *get_node(maze_t *maze, int x, int y) 
{
    // checking if passed parameters are null
    if ((maze != NULL) && (maze->maze_nodes != NULL)) { 
	// checking if passed x and y are within the bounds of the maze struct
	if ((x >= 0) && (x < maze->W) && (y >= 0) && (y < maze->H)) {  
            return maze->maze_nodes[x + y * maze->W];
	}
    }
    return NULL;    
}


int get_avatar(node_t *node) {
	return node->avatar;
}

void set_avatar(node_t *node, int avatar) {
	node->avatar = avatar;
}

int get_height(maze_t *maze) {
	return maze->H;
}

int get_width(maze_t *maze) {
	return maze->W;
}

int get_node_x(node_t *node) {
	return node->x;
}

int get_node_y(node_t *node) {
	return node->y;
}

int get_node_index(maze_t *maze, node_t *node) {
	return node->x + node->y * maze->W;
}

int get_L1_distance(node_t *node1, node_t *node2) {
	return abs(node1->x - node2->x) + abs(node1->y - node2->y);
}

/**************** check_node ****************/
int check_node_dir(maze_t *maze, node_t *node, int dir) //return an int based on what the node is pointing to
//return 0 if NULL
//return 1 if node is pointing to unknown placeholder node
//return 2 if node is pointing to wall placeholder node
//return 3 if node has a connection to another node that we already discovered/inferred
{
   
    node_t *dir_node = NULL;

    if (dir == 0) {
       dir_node = node->w;
    } else if (dir == 1) {
       dir_node = node->n;
    } else if (dir == 2) {
       dir_node = node->e;
    } else {
       dir_node = node->s; 
    } 

    if (dir_node == NULL)
    {
        return 0; // error conditions: node cannot be NULL;
    }
    if (dir_node == maze->unknown)
    {
        return 1;
    }
    if (dir_node == maze->wall)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

node_t *get_neighbor(maze_t *maze, node_t *node, int dir) {
  if (check_node_dir(maze, node, dir) != 2) {	
    if (dir == 0) {
      return get_node(maze, node->x - 1, node->y); 
    } else if (dir == 1) {
      return get_node(maze, node->x, node->y - 1);
    } else if (dir == 2) {
      return get_node(maze, node->x + 1, node->y);
    } else if (dir == 3) {
      return get_node(maze, node->x, node->y + 1);
    } 
  }
  return NULL;
}

/**************** set_wall ****************/
void set_wall(node_t *node, maze_t *maze, const int dir) //dir will indicate which direction we want to place a wall
{
    int indexInArr; // = x + y * W;
    int otherX;     //the x coordinate of the node on the other side of the wall
    int otherY;     //the y coordinate of the node on the other side of the wall
    node_t *otherNode;
    if (dir == 1) //set wall at north
    {
        node->n = maze->wall;
        //set corresponding wall on other node
        otherX = node->x;
        otherY = node->y - 1;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        otherNode->s = maze->wall;
    }
    if (dir == 2) //set wall at east
    {
        node->e = maze->wall;
        //set corresponding wall on other node
        otherX = node->x + 1;
        otherY = node->y;
	indexInArr = otherX + otherY * maze->W;
	otherNode = maze->maze_nodes[indexInArr];
        otherNode->w = maze->wall;
    }
    if (dir == 3) //set wall at south
    {
        node->s = maze->wall;
        //set corresponding wall on other node
        otherX = node->x;
        otherY = node->y + 1;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        otherNode->n = maze->wall;
    }
    if (dir == 0) //set wall at west
    {
        node->w = maze->wall;
        //set corresponding wall on other node
        otherX = node->x - 1;
        otherY = node->y;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        otherNode->e = maze->wall;
    }
}

/**************** set_connection ****************/
void set_connection(node_t *node, maze_t *maze, const int dir) //dir will indicate which direction we want to me a connection
{
    int indexInArr; // = x + y * W;
    int otherX;     //the x coordinate of the node on the other side of the wall
    int otherY;     //the y coordinate of the node on the other side of the wall
    node_t *otherNode;
    if (dir == 1) //set connection at north node
    {
        otherX = node->x;
        otherY = node->y - 1;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        node->n = otherNode;
        otherNode->s = node;
        //set corresponding connection on other node
    }
    if (dir == 2) //set connection at east node
    {
        otherX = node->x + 1;
        otherY = node->y;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        node->e = otherNode;
        otherNode->w = node;
        //set corresponding connection on other node
    }
    if (dir == 3) //set connection at south node
    {
        otherX = node->x;
        otherY = node->y + 1;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        node->s = otherNode;
        otherNode->n = node;
        //set corresponding connection on other node
    }
    if (dir == 0) //set connection at west node
    {
        otherX = node->x - 1;
        otherY = node->y;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        node->w = otherNode;
        otherNode->e = node;
        //set corresponding connection on other node
    }
}

/**************** maze_delete ****************/
void maze_delete(maze_t *maze)
{
    if (maze != NULL)
    {
        int numOfNodes = maze->W * maze->H;
        for (int i = 0; i < numOfNodes; i++)
        {
            count_free(maze->maze_nodes[i]); // free the node
        }
        count_free(maze->unknown);
        count_free(maze->wall);
        count_free(maze->maze_nodes);
        count_free(maze); //free the maze struct itself
        maze = NULL;
    }
#ifdef MEMTEST
    count_report(stdout, "End of maze_delete");
#endif
}
