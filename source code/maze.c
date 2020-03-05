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
    const int x, y; // the x and y coordinates of the maze node
    struct node *n; // pointer to mazenode's north node
    struct node *s; // pointer to mazenode's south node
    struct node *e; // pointer to mazenode's east node
    struct node *w; // pointer to mazenode's west node
} node_t;

/**************** global types ****************/
typedef struct maze
{
    struct node *unknown;     //pointer for representing node's that have not been discovered yet
    struct node *wall;        //pointer for representing walls between nodes
    struct node **maze_nodes; // array of mazenodes for the maze discovered/seen/deduced so far
    const int H, W;           // height and width of maze
} maze_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see maze.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static node_t *node_new(const int x, const int y, const int border); //use border int to determine where the wall should be given a border node

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
        maze->wall = node_new(-1, 0);                                           //initialize placeholder node for representing walls between nodes
        maze->unknown = node_new(0, -1);                                        //initialize placeholder node for representing directions that have not been discovered/are unknown
        maze->H = height;                                                       //height of maze
        maze->W = width;                                                        //width of maze
        node_t **mazeNodes = count_malloc((height * width) * (sizeof(node_t))); //check with jacob if it is sizeof pointer or just sizeof node
        int numOfNodes = height * width;
        int x;
        int y;
        for (int i = 0; i < numOfNodes; i++) //initialize mazeNodes
        {                                    //the nodes will be stored from bottom to top, left to right
            x = i % width;                   //x coordinate
            y = i / width;                   //y coordinate
            if (x == 0 && y == 0)
            {
                mazeNodes[i] = node_new(x, y, 3); //south-west border node
                mazeNode[i]->w = wall;
                continue;
            }
            if (x == width - 1 && y == 0)
            {
                mazeNodes[i] = node_new(x, y, 3); //south-east border node
                mazeNode[i]->e = wall;
                continue;
            }
            if (x == 0 && y == height - 1)
            {
                mazeNodes[i] = node_new(x, y, 1); //north-west border node
                mazeNode[i]->w = wall;
                continue;
            }
            if (x == width - 1 && y == height - 1)
            {
                mazeNodes[i] = node_new(x, y, 1); //north-east border node
                mazeNode[i]->e = wall;
                continue;
            }
            if (x == 0)
            {
                mazeNodes[i] = node_new(x, y, 4); //west border node
                continue;
            }
            if (x == width - 1)
            {
                mazeNodes[i] = node_new(x, y, 2); //east border node
                continue;
            }
            if (y == 0)
            {
                mazeNodes[i] = node_new(x, y, 3); //south border node
                continue;
            }
            if (y == height - 1)
            {
                mazeNodes[i] = node_new(x, y, 1); //north border node
                continue;
            }
            mazeNodes[i] = node_new(x, y, 0); //node is not surrounded by border
        }
        return maze;
    }
}

/**************** node_new ****************/
/* Allocate and initialize a mazenode */
// the 'static' modifier means this function is not visible
// outside this file
static node_t *node_new(const int x, const int y, const int border)
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
        node->n = unknown; //default will point to unknown
        node->s = unknown;
        node->w = unknown;
        node->e = unknown;
        if (border == 1) //node is on north border, therefore we know a wall is already there
        {
            node->n = wall;
        }
        if (border == 2) //node is on east border, therefore we know a wall is already there
        {
            node->e = wall;
        }
        if (border == 3) //node is on south border, therefore we know a wall is already there
        {
            node->s = wall;
        }
        if (border == 4) //node is on west border, therefore we know a wall is already there
        {
            node->w = wall;
        }
        if (border == 0) //node is not on a border, therefore we do not know where any walls are
        {                //do nothing
        }
        return node;
    }
}

/**************** check_node ****************/
int check_node(node_t *node) //return an int based on what the node is pointing to
//return 0 if NULL
//return 1 if node is pointing to unknown placeholder node
//return 2 if node is pointing to wall placeholder node
//return 3 if node has a connection to another node that we already discovered/inferred
{
    if (node == NULL)
    {
        return 0; // error conditions: node cannot be NULL;
    }
    if (node == unknown)
    {
        return 1;
    }
    if (node == wall)
    {
        return 2;
    }
    else
    {
        return 3;
    }
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
        node->n = wall;
        //set corresponding wall on other node
        otherX = x;
        otherY = y + 1;
        indexInArr = x + y * W;
        otherNode = maze_nodes[indexInArr];
        otherNode->s = wall;
    }
    if (node == 2) //set wall at east
    {
        node->e = wall;
        //set corresponding wall on other node
        otherX = x + 1;
        otherY = y;
        indexInArr = x + y * W;
        otherNode = maze_nodes[indexInArr];
        otherNode->w = wall;
    }
    if (node == 3) //set wall at south
    {
        node->s = wall;
        //set corresponding wall on other node
        otherX = x;
        otherY = y - 1;
        indexInArr = x + y * W;
        otherNode = maze_nodes[indexInArr];
        otherNode->n = wall;
    }
    if (node == 4) //set wall at west
    {
        node->w = wall;
        //set corresponding wall on other node
        otherX = x - 1;
        otherY = y;
        indexInArr = x + y * W;
        otherNode = maze_nodes[indexInArr];
        otherNode->e = wall;
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
        otherX = x;
        otherY = y + 1;
        indexInArr = x + y * W;
        otherNode = maze_nodes[indexInArr];
        node->n = otherNode;
        otherNode->s = node;
        //set corresponding connection on other node
    }
    if (node == 2) //set connection at east node
    {
        otherX = x + 1;
        otherY = y;
        indexInArr = x + y * W;
        otherNode = maze_nodes[indexInArr];
        node->e = otherNode;
        otherNode->w = node;
        //set corresponding connection on other node
    }
    if (node == 3) //set connection at south node
    {
        otherX = x;
        otherY = y - 1;
        indexInArr = x + y * W;
        otherNode = maze_nodes[indexInArr];
        node->s = otherNode;
        otherNode->n = node;
        //set corresponding connection on other node
    }
    if (node == 4) //set connection at west node
    {
        otherX = x - 1;
        otherY = y;
        indexInArr = x + y * W;
        otherNode = maze_nodes[indexInArr];
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
        int numOfNodes = x * y;
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
