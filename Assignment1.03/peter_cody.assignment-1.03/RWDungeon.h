//
//  Dungeon.h
//  
//
//  Created by Cody Peter on 9/25/18.
//

#ifndef RWDungeon_h
#define RWDungeon_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

//consts
#define ROOM                  '.'
#define ROCK                  ' '
#define TUNNEL                '#'
#define PLAYER                '@'
#define DUNGEON_X              80
#define DUNGEON_Y              21
#define MIN_ROOMS              5
#define MAX_ROOMS              9
#define ROOM_MIN_X             4
#define ROOM_MIN_Y             2
#define ROOM_MAX_X             14
#define ROOM_MAX_Y             8


int num_rooms;
int roomList[20][4];
int pcX;
int pcY;


typedef struct {
    char tile;
    int density;
    int nonTunnelNPC;
    int tunnelNPC;
    int x;
    int y;
}tile_struct;
tile_struct board[DUNGEON_Y][DUNGEON_X];

//functions
void createRooms();
bool addRoom(int width, int height, int x, int y);
void createDungeon();
void addHallways();
void tunnelToPoint(int xStart, int yStart, int xPoint, int yPoint);
void tunnelSide(int x1, int x2, int y);
void tunnelUp(int y1, int y2, int x);
int rand_range(int min, int max);
void print_dungeon();
int save_dungeon(char* path, int version);
int load_dungeon(char* path, int* version);
int main_dungeon(int argc, char* argv[]);

#endif /* RwDungeon_h */
