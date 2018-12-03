//
//  Dungeon.c
//  
//
//  Created by Cody Peter on 8/29/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

int Width = 80;
int Height = 21;
int roomIndex = 0;
char board[22][81];
int roomList[20][4];

void createRooms();
bool addRoom(int width, int height, int x, int y);
void tunnelToPoint(int xStart, int yStart, int xPoint, int yPoint);
void tunnelSide(int x1, int x2, int y);
void tunnelUp(int y1, int y2, int x);
int rand_range(int min, int max);


void printDungeon()
{
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            if (i == 0 || i == Height-1) {
                printf("-");
            } else if (j == 0 || j == Width-1) {
                printf("|");
            } else {
                printf("%c", board[i][j]);
            }
        }
    }
}


void createDungeon() {
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            board[i][j] = ' ';
        }
    }
    createRooms();
}


void createRooms() {
    int numRooms = rand_range(5, 10);
    bool done = false;
    printf("num rooms: %d\n", numRooms);


    for (int i = 0; i < numRooms; i++) {
        done = false;
        while (!done) {
            int width = rand_range(3,10);
            int height = rand_range(2, 8);
            int x = rand_range(2, 78-width);
            int y = rand_range(2, 19-height);
//             = rand_range(1, 77);
//             = rand_range(1, 19);
            //printf("width: %d, height: %d, x: %d, y: %d\n", width, height, x, y);
            if (addRoom(width, height, x, y)) {
                done = true;
                roomList[roomIndex][0] = x;
                roomList[roomIndex][1] = y;
                roomList[roomIndex][2] = width;
                roomList[roomIndex][3] = height;
                roomIndex++;
            }
        }
     }
}


bool addRoom(int width, int height, int x, int y) {
    if (x + width > Width || y + height > Height) {
        return false;
    }
    for (int i = y; i < height + y; i++) {
        for (int j = x; j < width + x; j++) {
            if (board[i][j] != ' ') {
                return false;
            }
            else if (board[i-2][j] == '.') {
                return false;
            }
            else if (board[i+2][j] == '.'){
                return false;
            }
            else if (board[i][j-2] == '.'){
                return false;
            }
            else if (board[i][j+2] == '.'){
                return false;
            }
            else if (board[i-1][j-1] == '.'){
                return false;
            }
            else if (board[i+1][j-1] == '.'){
                return false;
            }
            else if (board[i+1][j+1] == '.'){
                return false;
            }
            else if (board[i-1][j+1] == '.'){
                return false;
            }
        }
    }
    
    for (int i = x; i < (width + x); i++) {
        for (int j = y; j < (height + y); j++) {
            board[j][i] = '.';
        }
    }
    return true;
}

void addHallways() {
    int centerX = Width / 2 - 1;
    int centerY = Height / 2 - 1;
    centerY = 2;
//    centerX = rand_range(2, Width-2);
//    centerY = rand_range(2, Height-2);
    
    for (int i = 0; i < roomIndex; i++) {
        printf("room x: %d\n", roomList[i][1]);
        int currentX = roomList[i][0];
        int currentY = roomList[i][1];
        
        tunnelToPoint(currentX, currentY, centerX, centerY);
    }
}


void tunnelToPoint(int xStart, int yStart, int xPoint, int yPoint) {
    if (xPoint > xStart) {
        tunnelSide(xStart, xPoint, yStart);
        if (yPoint >= yStart) {
            tunnelUp(yStart, yPoint, xPoint);
        } else {
            tunnelUp(yPoint, yStart, xPoint);
        }
    } else {
        tunnelSide(xPoint, xStart, yStart);
        if (yPoint >= yStart) {
            tunnelUp(yStart, yPoint, xPoint);
        } else {
            tunnelUp(yPoint, yStart, xPoint);
        }
    }
}


void tunnelSide(int x1, int x2, int y){
    
    for (int i = x1; i <= x2; i++){
        if (board[y][i] != '.'){
            board[y][i] = '#';
        }
    }
}

void tunnelUp(int y1, int y2, int x){
    
    for (int i = y1; i <= y2; i++){
        if (board[i][x] != '.'){
            board[i][x] = '#';
        }
    }
}

int rand_range(int min, int max) {
    return(rand() % (max-min + 1)) + min;
}

int main()
{
    srand ( time(NULL) );
    createDungeon();
    addHallways();
    printDungeon();
    return 0;
}
