/* ========================================================================== */
/* File: amazing.h
 *
 * Project name: CS50 Amazing Project
 * Component name: amazing header
 *
 * Primary Author: Charles C. Palmer
 * Date Created: Mon Jan 2 2012
 *
 * Last Update by: Ira Ray Jenkins - Sun May 11, 2014
 *
 * Updated by Xia Zhou, August 2016
 * Updated by Temi Prioleau, February 2020
 *
 * This file contains the common defines for the Amazing Project.
 *
 */
/* ========================================================================== */
#ifndef __AMAZING_H
#define __AMAZING_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include <stdint.h>                          // uint32_t

// ---------------- Constants
/* Server parameters */
#define AM_SERVER_PORT  "17235"              // Server management port, ASCII "CS"
#define AM_MAX_MESSAGE     128               // max message length in bytes
#define AM_MAX_DIFFICULTY    9               // max # of maze difficulty levels
#define AM_MAX_AVATAR       10               // max # of avatars for any MazePort
#define AM_MAX_MOVES      1000               // max # of moves for all Avatars
#define AM_WAIT_TIME       600               // seconds waited before server dies

/* Avatar constants */
#define M_WEST           0
#define M_NORTH          1
#define M_SOUTH          2
#define M_EAST           3
#define M_NUM_DIRECTIONS 4
#define M_NULL_MOVE      8

/* Client-server protocol message type values */
#define AM_ERROR_MASK             0x80000000
#define AM_MAZE_SOLVED            0x00010000
#define AM_INIT                   0x00000001
#define AM_INIT_OK                0x00000002
#define AM_AVATAR_READY           0x00000004
#define AM_AVATAR_TURN            0x00000008
#define AM_AVATAR_MOVE            0x00000010
#define AM_INIT_FAILED           (0x00000020 | AM_ERROR_MASK)
#define AM_AVATAR_OUT_OF_TURN    (0x00000040 | AM_ERROR_MASK)
#define AM_NO_SUCH_AVATAR        (0x00000080 | AM_ERROR_MASK)
#define AM_TOO_MANY_MOVES        (0x00000100 | AM_ERROR_MASK)
#define AM_SERVER_DISK_QUOTA     (0x00000200 | AM_ERROR_MASK)
#define AM_SERVER_OUT_OF_MEM     (0x00000400 | AM_ERROR_MASK)
#define AM_UNKNOWN_MSG_TYPE      (0x01000000 | AM_ERROR_MASK)
#define AM_SERVER_TIMEOUT        (0x02000000 | AM_ERROR_MASK)
#define AM_UNEXPECTED_MSG_TYPE   (0x04000000 | AM_ERROR_MASK)

/* AM_INIT failure ErrNums */
#define AM_INIT_ERROR_MASK       (0x40000000 | AM_ERROR_MASK)
#define AM_INIT_TOO_MANY_AVATARS (0x00000001 | AM_INIT_ERROR_MASK)
#define AM_INIT_BAD_DIFFICULTY   (0x00000002 | AM_INIT_ERROR_MASK)

// ---------------- Structures/Types
/* XY-coordinate position */
typedef struct XYPos
{
    uint32_t x;
    uint32_t y;
} XYPos;

/* Maze avatar */
typedef struct Avatar
{
    int fd; //comm_sock # 
    XYPos pos;        //avatar's position 
    char *program;	  // this program's name
    int AvatarId;     //avatar ID
    int nAvatars;     //number of avatars 
    int Difficulty;   // maze level
    char *hostname;   // server hostname
    int MazePort;	  // server port
    char* logfilename;//log file 
    bool endgame;      //if the game is over or not 
} Avatar;

/* AM Message description */
typedef struct AM_Message
{
    uint32_t type;

    /* Define a union for all the message types that have parameters. Messages
     * with no parameters don't need to be part of this union. Defined as an
     * anonymous union to facilitate easier access.
     */
    union
    {
        /* AM_INIT */
        struct
        {
            uint32_t nAvatars;
            uint32_t Difficulty;
        } init;

        /* AM_INIT_OK */
        struct
        {
            uint32_t MazePort;
            uint32_t MazeWidth;
            uint32_t MazeHeight;
        } init_ok;

        /* AM_INIT_FAILED */
        struct
        {
            uint32_t ErrNum;
        } init_failed;

        /* AM_AVATAR_READY */
        struct
        {
            uint32_t AvatarId;
        } avatar_ready;

        /* AM_AVATAR_TURN */
        struct
        {
            uint32_t TurnId;
            XYPos    Pos[AM_MAX_AVATAR];
        } avatar_turn;

        /* AM_AVATAR_MOVE */
        struct
        {
            uint32_t AvatarId;
            uint32_t Direction;
        } avatar_move;

        /* AM_MAZE_SOLVED */
        struct
        {
            uint32_t nAvatars;
            uint32_t Difficulty;
            uint32_t nMoves;
            uint32_t Hash;
        } maze_solved;

        /* AM_UNKNOWN_MSG_TYPE */
        struct
        {
            uint32_t BadType;
        } unknown_msg_type;
    };
} AM_Message;

// ---------------- Public Variables
/* add your variables here if any*/

// ---------------- Prototypes/Macros
#define IS_AM_ERROR(code) ((code) & (AM_ERROR_MASK))

#endif // __AMAZING_H
