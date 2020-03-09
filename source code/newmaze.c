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
#include "./libcs50/counters.h"
#include "./libcs50/memory.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct node
{
    int x, y;       // the x and y coordinates of the maze node
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
    int H, W;                 // height and width of maze
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
        maze->wall = node_new(maze, -1, 0, 0); //initialize placeholder node for representing walls between nodes
        node_t *wall = maze->wall;
        maze->unknown = node_new(maze, 0, -1, 0);                             //initialize placeholder node for representing directions that have not been discovered/are unknown
        maze->H = height;                                                     //height of maze
        maze->W = width;                                                      //width of maze
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
        node->x = x; //node's y coordinate
        node->y = y; //node's y coordinate
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
//Translates x and y coordinates to the index number of the maze_nodes stored in the maze
//returns the node located at the x and y coordinates given in the parameters
//else return null if node does not exist in maze_nodes
node_t *get_node(maze_t *maze, int x, int y)
{
    // checking if passed parameters are null
    if ((maze != NULL) && (maze->maze_nodes != NULL))
    {
        // checking if passed x and y are within the bounds of the maze struct
        if ((x >= 0) && (x < maze->W) && (y >= 0) && (y < maze->H))
        {
            return maze->maze_nodes[x + y * maze->W];
        }
    }
    return NULL;
}

/**************** get_index ****************/
//Translates x and y coordinates to the index number of the maze_nodes stored in the maze
//returns the index number of where these x and y coordinates's node is stored in the array of nodes
//else return -1 if node does not exist in maze_nodes
int get_index(maze_t *maze, int x, int y)
{
    int index;
    // checking if passed parameters are null
    if ((maze != NULL) && (maze->maze_nodes != NULL))
    {
        // checking if passed x and y are within the bounds of the maze struct
        if ((x >= 0) && (x < maze->W) && (y >= 0) && (y < maze->H))
        {
            index = x + y * maze->W;
            return index;
        }
    }
    return -1;
}

/**************** get_avatar ****************/
//returns an integer that represents the avatar's ID of the avatar currently on the node
//if there is no avatar, return -1
int get_avatar(node_t *node)
{
    return node->avatar;
}

/**************** set_avatar ****************/
//set an integer on a node that represents the avatar's ID of the avatar currently on the node
void set_avatar(node_t *node, int avatar)
{
    node->avatar = avatar;
}

/**************** get_height ****************/
//returns an integer that represents the height of the maze
int get_height(maze_t *maze)
{
    return maze->H;
}

/**************** get_width ****************/
//returns an integer that represents the width of the maze
int get_width(maze_t *maze)
{
    return maze->W;
}

/**************** get_node_x ****************/
//returns an integer that represents the X coordinate of a node
int get_node_x(node_t *node)
{
    return node->x;
}

/**************** get_node_y ****************/
//returns an integer that represents the Y coordinate of a node
int get_node_y(node_t *node)
{
    return node->y;
}

/**************** check_node_dir ****************/
int check_node_dir(maze_t *maze, node_t *node, int dir) //return an int based on what the node is pointing to
//return 0 if NULL
//return 1 if node is pointing to unknown placeholder node
//return 2 if node is pointing to wall placeholder node
//return 3 if node has a connection to another node that we already discovered/inferred
{

    node_t *dir_node = NULL;

    if (dir == 0)
    {
        dir_node = node->w;
    }
    else if (dir == 1)
    {
        dir_node = node->n;
    }
    else if (dir == 2)
    {
        dir_node = node->e;
    }
    else
    {
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

/**************** further_deduce_connection ****************/
//recursively checks if there is a connection that can be deduced if
//we know there are was a dead end previously, and therefore the following nodes cannot be dead ends
//until we reach a node with less than 2 walls
void further_deduce_connection(node_t *node, maze_t *maze)
{
    int wallcount = 0; // count of walls next to node
    if (node->n == maze->wall)
    {
        wallcount++;
    }
    if (node->e == maze->wall)
    {
        wallcount++;
    }
    if (node->s == maze->wall)
    {
        wallcount++;
    }
    if (node->w == maze->wall)
    {
        wallcount++;
    }
    if (wallcount == 2) //if there are two walls, we know the node being pointed to that is unknown must not be a wall!
    {
        if (node->n == maze->unknown)
        {
            set_connection(node, maze, 1);                                                           //make a north connection
            further_deduce_connection(get_node(maze, get_node_x(node), get_node_y(node) + 1), maze); //deduce further connections
        }
        if (node->e == maze->unknown)
        {
            set_connection(node, maze, 2);                                                           //make a east connection
            further_deduce_connection(get_node(maze, get_node_x(node) + 1, get_node_y(node)), maze); //deduce further connections
        }
        if (node->s == maze->unknown)
        {
            set_connection(node, maze, 3);                                                           //make a south connection
            further_deduce_connection(get_node(maze, get_node_x(node), get_node_y(node) - 1), maze); //deduce further connections
        }
        if (node->w == maze->unknown)
        {
            set_connection(node, maze, 0);                                                           //make a west connection
            further_deduce_connection(get_node(maze, get_node_x(node) - 1, get_node_y(node)), maze); //deduce further connections
        }
    }
}

/**************** deduce_connection ****************/
//checks if there is a connection that can be deduced if
//we know there are three walls for a given node (a deadend)
void deduce_connection(node_t *node, maze_t *maze)
{
    int wallcount = 0; // count of walls next to node
    if (node->n == maze->wall)
    {
        wallcount++;
    }
    if (node->e == maze->wall)
    {
        wallcount++;
    }
    if (node->s == maze->wall)
    {
        wallcount++;
    }
    if (node->w == maze->wall)
    {
        wallcount++;
    }
    if (wallcount == 3) //if there are three walls, we know the node being pointed to that is unknown must not be a wall!
    {
        if (node->n == maze->unknown)
        {
            set_connection(node, maze, 1);                                                           //make a north connection
            further_deduce_connection(get_node(maze, get_node_x(node), get_node_y(node) + 1), maze); //deduce further connections
        }
        if (node->e == maze->unknown)
        {
            set_connection(node, maze, 2);                                                           //make a east connection
            further_deduce_connection(get_node(maze, get_node_x(node) + 1, get_node_y(node)), maze); //deduce further connections
        }
        if (node->s == maze->unknown)
        {
            set_connection(node, maze, 3);                                                           //make a south connection
            further_deduce_connection(get_node(maze, get_node_x(node), get_node_y(node) - 1), maze); //deduce further connections
        }
        if (node->w == maze->unknown)
        {
            set_connection(node, maze, 0);                                                           //make a west connection
            further_deduce_connection(get_node(maze, get_node_x(node) - 1, get_node_y(node)), maze); //deduce further connections
            //actually just add deduce wall into set_connection
        }
    }
}

//helper function for check_loop
void check_loopR(node_t *node, maze_t *maze, counters_t *check_for_loop, const int dir)
{
    if (counters_add(check_for_loop, get_index(maze, get_node_x(node), get_node_y(node))) > 1)
    {
        return; //we have found a loop, therefore we must break from iterating through this path!
    }           //add node to nodes traveled so far, stored as index in maze's array of nodes
    //all we are doing is building out the tree based on this assumed connection
    //build out tree
    if (node->n != maze->wall && node->n != maze->unknown && dir != 3)
    {
        check_loopR(node->n, maze, check_for_loop, 1); //1 is north, the direction we moved in, which we will not check backwards when
    }
    if (node->s != maze->wall && node->s != maze->unknown && dir != 1)
    {
        check_loopR(node->s, maze, check_for_loop, 3); //3 is south, the direction we moved in, which we will not check backwards when
    }
    if (node->e != maze->wall && node->e != maze->unknown && dir != 0)
    {
        check_loopR(node->e, maze, check_for_loop, 2); //2 is east, the direction we moved in, which we will not check backwards when
    }
    if (node->w != maze->wall && node->w != maze->unknown && dir != 2)
    {
        check_loopR(node->w, maze, check_for_loop, 0); //0 is west, the direction we moved in, which we will not check backwards when
    }
}
//determine whether or not there is a loop created as a result of assuming our unknown to be a connection
bool check_loop(node_t *node, maze_t *maze, counters_t *nodes_so_far, const int dir)
{
    //bool status = false; //we will assume there is no loop created by this potential connection
    //however if there is, we will update status to be true, meaning there is a loop created as a result of the assumption
    counters_t *check_for_loop = counters_new();
    //add all of nodes from nodes so far into new counterset, since we should not encounter any of them
    //if we do not go in the same direction from a previous iteration (definition of a loop)
    counters_iterate(nodes_so_far, check_for_loop, insert_node);                       //add node to nodes traveled so far, stored as respective index number in maze's array of nodes
    counters_add(check_for_loop, get_index(maze, get_node_x(node), get_node_y(node))); //add node to nodes traveled so far, stored as index in maze's array of nodes
    //all we are doing is building out the tree based on this assumed connection
    //build out tree
    if (node->n != maze->wall && node->n != maze->unknown && dir != 3)
    {
        check_loopR(node->n, maze, check_for_loop, 1); //1 is north, the direction we moved in, which we will not check backwards when
    }
    if (node->s != maze->wall && node->s != maze->unknown && dir != 1)
    {
        check_loopR(node->s, maze, check_for_loop, 3); //3 is south, the direction we moved in, which we will not check backwards when
    }
    if (node->e != maze->wall && node->e != maze->unknown && dir != 0)
    {
        check_loopR(node->e, maze, check_for_loop, 2); //2 is east, the direction we moved in, which we will not check backwards when
    }
    if (node->w != maze->wall && node->w != maze->unknown && dir != 2)
    {
        check_loopR(node->w, maze, check_for_loop, 0); //0 is west, the direction we moved in, which we will not check backwards when
    }
    //interating through the nodes discovered
    //use counters_iterate
    int *counter = malloc(sizeof(int));
    *counter = 0;
    counters_iterate(check_for_loop, counter, isloop);
    counters_delete(check_for_loop);
    count_free(counter);
    if (*counter == 1)
    {
        return true;//loop found!
    }
    else{
        return false;//loop could not be found
    }
}

/**************** deduce_wallR ****************/
//recursively checks if there is a wall that can be deduced if
//we know a loop would exist if a particular node has a connection, which is impossible
//since our maze has no loops!
//use a dfs
void deduce_wallR(node_t *node, maze_t *maze, counters_t *nodes_so_far, const int dir)
{
    counters_add(nodes_so_far, get_index(maze, get_node_x(node), get_node_y(node))); //add node to nodes traveled so far, stored as index in maze's array of nodes

    //interate throughout the maze's nodes, checking if any unknowns will produce loops when
    //treated as a connection
    //dir denotes the direction we just traveled in, which we do not want to interate to because
    //we will end up in an infinite loop of iterating back and forth
    if (node->n == maze->unknown && dir != 3)
    {
        try_connection(node, maze, 1);
        if (check_loop(node->n, maze, nodes_so_far, 1))
        {
            set_wall(node, maze, 1);
        }
        else
        {
            set_unknown(node, maze, 1);
        }
    }
    if (node->s == maze->unknown && dir != 1)
    {
        try_connection(node, maze, 3);
        if (check_loop(node->s, maze, nodes_so_far, 3))
        {
            set_wall(node, maze, 3);
        }
        else
        {
            set_unknown(node, maze, 3);
        }
    }
    if (node->e == maze->unknown && dir != 0)
    {
        try_connection(node, maze, 2);
        if (check_loop(node->e, maze, nodes_so_far, 2))
        {
            set_wall(node, maze, 2);
        }
        else
        {
            set_unknown(node, maze, 2);
        }
    }
    if (node->w == maze->unknown && dir != 2)
    {
        try_connection(node, maze, 0);
        if (check_loop(node->w, maze, nodes_so_far, 0))
        {
            set_wall(node, maze, 0);
        }
        else
        {
            set_unknown(node, maze, 0);
        }
    }
    if (node->n != maze->wall && node->n != maze->unknown && dir != 3)
    {
        deduce_wallR(node->n, maze, nodes_so_far, 1); //1 is north, the direction we moved in, which we will not check backwards when
        //interating through the maze
    }
    if (node->s != maze->wall && node->s != maze->unknown && dir != 1)
    {
        deduce_wallR(node->s, maze, nodes_so_far, 3); //3 is south, the direction we moved in, which we will not check backwards when
        //interating through the maze
    }
    if (node->e != maze->wall && node->e != maze->unknown && dir != 0)
    {
        deduce_wallR(node->e, maze, nodes_so_far, 2); //2 is east, the direction we moved in, which we will not check backwards when
        //interating through the maze
    }
    if (node->w != maze->wall && node->w != maze->unknown && dir != 2)
    {
        deduce_wallR(node->w, maze, nodes_so_far, 0); //0 is west, the direction we moved in, which we will not check backwards when
        //interating through the maze
    }

    //once we hit an unknown leaf
    //call a function that checks if a loop would be created by setting one of the unknown leaves
    //to a temporary connection. If that does create a loop, we know that unknown leaf must be a wall
}

/**************** deduce_wall ****************/
//recursively checks if there is a wall that can be deduced if
//we know a loop would exist if a particular node has a connection, which is impossible
//since our maze has no loops!
//use a dfs
void deduce_wall(node_t *node, maze_t *maze)
{ //use a counterset to hold our nodes traveled so far, if we end up finding a repeat, we know we entered a loop
    counters_t *nodes_so_far = counters_new();
    counters_add(nodes_so_far, get_index(maze, get_node_x(node), get_node_y(node))); //add node to nodes traveled so far, stored as index in maze's array of nodes

    //now build out the current path traveled by our avatar as a tree
    //the bottom 'leaves' will be either unknown or a wall
    //recursiveley build out in DFS fashion!
    //the counterset will hold the index's of each node
    if (node->n == maze->unknown)
    {
        try_connection(node, maze, 1);
        if (check_loop(node->n, maze, nodes_so_far, 1))
        {
            set_wall(node, maze, 1);
        }
        else
        {
            set_unknown(node, maze, 1);
        }
    }
    if (node->s == maze->unknown)
    {
        try_connection(node, maze, 3);
        if (check_loop(node->s, maze, nodes_so_far, 3))
        {
            set_wall(node, maze, 3);
        }
        else
        {
            set_unknown(node, maze, 3);
        }
    }
    if (node->e == maze->unknown)
    {
        try_connection(node, maze, 2);
        if (check_loop(node->e, maze, nodes_so_far, 2))
        {
            set_wall(node, maze, 2);
        }
        else
        {
            set_unknown(node, maze, 2);
        }
    }
    if (node->w == maze->unknown)
    {
        try_connection(node, maze, 0);
        if (check_loop(node->w, maze, nodes_so_far, 0))
        {
            set_wall(node, maze, 0);
        }
        else
        {
            set_unknown(node->n, maze, 0);
        }
    }
    if (node->n != maze->wall && node->n != maze->unknown)
    {
        deduce_wallR(node->n, maze, nodes_so_far, 1); //1 is north, the direction we moved in, which we will not check backwards when
        //interating through the maze
    }
    if (node->s != maze->wall && node->s != maze->unknown)
    {
        deduce_wallR(node->s, maze, nodes_so_far, 3); //3 is south, the direction we moved in, which we will not check backwards when
        //interating through the maze
    }
    if (node->e != maze->wall && node->e != maze->unknown)
    {
        deduce_wallR(node->e, maze, nodes_so_far, 2); //2 is east, the direction we moved in, which we will not check backwards when
        //interating through the maze
    }
    if (node->w != maze->wall && node->w != maze->unknown)
    {
        deduce_wallR(node->w, maze, nodes_so_far, 0); //0 is west, the direction we moved in, which we will not check backwards when
        //interating through the maze
    }

    counters_delete(nodes_so_far);

    //once we hit an unknown leaf
    //call a function that checks if a loop would be created by setting one of the unknown leaves
    //to a temporary connection. If that does create a loop, we know that unknown leaf must be a wall
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

    //Now I need to check if other connections can be deduced from the wall inserted for both nodes
    deduce_connection(node, maze);
    deduce_connection(otherNode, maze);
}

/**************** set_connection ****************/
void set_connection(node_t *node, maze_t *maze, const int dir) //dir will indicate which direction we want to make a connection
{
    int indexInArr; // = x + y * W;
    int otherX;     //the x coordinate of the node on the other side
    int otherY;     //the y coordinate of the node on the other side
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
    deduce_wall(node, maze);
}

/**************** try_connection ****************/
void try_connection(node_t *node, maze_t *maze, const int dir) //dir will indicate which direction we want to make a connection
{
    int indexInArr; // = x + y * W;
    int otherX;     //the x coordinate of the node on the other side
    int otherY;     //the y coordinate of the node on the other side
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
    } //since we are just trying a connection, we cannot deduce walls off it directly
    // until we can prove it is a connection
}

/**************** set_unknown ****************/
void set_unknown(node_t *node, maze_t *maze, const int dir) //dir will indicate which direction we want to me a connection
{
    int indexInArr; // = x + y * W;
    int otherX;     //the x coordinate of the node on the other side
    int otherY;     //the y coordinate of the node on the other side
    node_t *otherNode;
    if (dir == 1) //set unknown at north node
    {
        otherX = node->x;
        otherY = node->y - 1;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        node->n = maze->unknown;
        otherNode->s = maze->unknown;
        //set corresponding unknown on other node
    }
    if (dir == 2) //set unknown at east node
    {
        otherX = node->x + 1;
        otherY = node->y;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        node->e = maze->unknown;
        otherNode->w = maze->unknown;
        //set corresponding unknown on other node
    }
    if (dir == 3) //set unknown at south node
    {
        otherX = node->x;
        otherY = node->y + 1;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        node->s = maze->unknown;
        otherNode->n = maze->unknown;
        //set corresponding unknown on other node
    }
    if (dir == 0) //set unknown at west node
    {
        otherX = node->x - 1;
        otherY = node->y;
        indexInArr = otherX + otherY * maze->W;
        otherNode = maze->maze_nodes[indexInArr];
        node->w = maze->unknown;
        otherNode->e = maze->unknown;
        //set corresponding unknown on other node
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
//helper function for inserting node's indexes of nodes seen so far into new counterset
void insert_node(void *arg, const int key, const int count)
{
    counters_t *toInsertInto = arg;
    counters_add(toInsertInto, key);
}

void isloop(void *arg, const int key, const int count)
{
    int *counter = arg;
    if (count > 1)
    {
        *counter = 1;
    }
}
