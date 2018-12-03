#include "RWDungeon.h"
#if (defined(__APPLE__))
#       include <libkern/OSByteOrder.h>
#       define htobe32(x) OSSwapHostToBigInt32(x)
#       define be32toh(x) OSSwapBigToHostInt32(x)
#endif

int main_dungeon(int argc, char* argv[]){
    //char* path;
    //char input[50];
    //int version = 0;

    //printf("Please enter your file name: ");
    //scanf("%s", input);

//    path = (char*) malloc ((strlen(getenv("HOME")) + strlen("/.rlg327/") + strlen(input) * sizeof(char*)));
//
//    strcat(path, getenv("HOME"));
//    strcat(path, "/.rlg327/");
//    strcat(path, input);
//
//
//    if (strcmp(argv[1], "--load") == 0) {
//        printf("getting board from: %s\n", path);
//        load_dungeon(path, &version);
//    }
//    else if (strcmp(argv[1], "--save") == 0){
//        srand ( time(NULL) );
//        createDungeon();
//        addHallways();
//        print_dungeon();
//        printf("Saving board to %s\n", path);
//        save_dungeon(path, version);
//    } else {
//        printf("pad arguments");
//        return -1;
//    }
//    free(path);
    srand ( time(NULL) );
    createDungeon();
    addHallways();
    return 0;
}


void createDungeon() {
    for (int i = 0; i < DUNGEON_Y; i++) {
        for (int j = 0; j < DUNGEON_X; j++) {
            if (i == 21 || i == 0 || j == 80 || j == 0){
                board[i][j].density = 255;
            }
            else {
                board[i][j].density = rand_range(1,254);
            }
            board[i][j].tile = ROCK;
            board[i][j].nonTunnelNPC = 255;
            board[i][j].tunnelNPC = 255;
            board[i][j].x = i;
            board[i][j].y = j;
            board[i][j].mon.type = -1;
        }
    }
    createRooms();
}


void createRooms() {
    int numRooms = rand_range(5, 10);
    bool done = false;
    int roomIndex = 0;

    for (int i = 0; i < numRooms; i++) {
        done = false;
        while (!done) {
            int width = rand_range(3,10);
            int height = rand_range(2, 8);
            int x = rand_range(2, 78-width);
            int y = rand_range(2, 19-height);
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
    num_rooms = roomIndex;
}


bool addRoom(int width, int height, int x, int y) {
    if (x + width > DUNGEON_X || y + height > DUNGEON_Y) {
        return false;
    }
    for (int i = y; i < height + y; i++) {
        for (int j = x; j < width + x; j++) {
            if (board[i][j].tile != ROCK) {
                return false;
            }
            else if (board[i-2][j].tile == ROOM) {
                return false;
            }
            else if (board[i+2][j].tile == ROOM){
                return false;
            }
            else if (board[i][j-2].tile == ROOM){
                return false;
            }
            else if (board[i][j+2].tile == ROOM){
                return false;
            }
            else if (board[i-1][j-1].tile == ROOM){
                return false;
            }
            else if (board[i+1][j-1].tile == ROOM){
                return false;
            }
            else if (board[i+1][j+1].tile == ROOM){
                return false;
            }
            else if (board[i-1][j+1].tile == ROOM){
                return false;
            }
        }
    }
    for (int i = x; i < (width + x); i++) {
        for (int j = y; j < (height + y); j++) {
            board[j][i].tile = ROOM;
            board[j][i].density = 0;
        }
    }
    return true;
}

void addHallways() {
    int centerX = DUNGEON_X / 2 - 1;
    int centerY = DUNGEON_Y / 2 - 1;
    centerY = 2;
    //    centerX = rand_range(2, Width-2);
    //    centerY = rand_range(2, Height-2);
    
    for (int i = 0; i < num_rooms; i++) {
        //printf("room x: %d\n", roomList[i][1]);
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
        if (board[y][i].tile != ROOM){
            board[y][i].tile = TUNNEL;
            board[y][i].density = 0;
        }
    }
}

void tunnelUp(int y1, int y2, int x){
    for (int i = y1; i <= y2; i++){
        if (board[i][x].tile != ROOM){
            board[i][x].tile = TUNNEL;
            board[i][x].density = 0;
        }
    }
}

int rand_range(int min, int max) {
    return(rand() % (max-min + 1)) + min;
}

void print_mon(int mon_type) {
    char c;
    switch (mon_type) {
        case 0:
            c = PLAYER;
            break;
        case 1:
            c = GIANT;
            break;
        case 2:
            c = HUMAN;
            break;
    }
    printf("%c", c);
}


void print_dungeon() {
    for (int i = 0; i < 21; i++){
        for (int j = 0; j < 80; j++){
            if (board[i][j].mon.type != -1) {
                //printf("mon");
                print_mon(board[i][j].mon.type);
            }
            else {
                printf("%c", board[i][j].tile);
            }
        }
        printf("\n");
    }
}


int save_dungeon(char* path, int version){

    char f_type[12] = "RLG327-F2018";
    uint32_t f_version = version;
    uint32_t f_size = 1702 + (num_rooms * 4);
    FILE *f;

    f = fopen(path, "w");
    if (f == NULL){
        printf("Couldn't save file at %s", path);
        return -1;
    }

    f_version = htobe32(f_version);
    f_size = htobe32(f_size);

    fwrite(f_type, 12, 1, f);
    fwrite(&f_version, 4, 1, f);
    fwrite(&f_size, 4, 1, f);
    fwrite(&pcX, 1, 1, f);
    fwrite(&pcY, 1, 1, f);

    //write hardness
    for (int i = 0; i < 21; i++) {
        for (int j = 0; j < 80; j++) {
            fwrite(&board[i][j].density, 1, 1, f);
        }
    }
    //room information
    for (int i = 0; i < num_rooms; i++){
        fwrite(&roomList[i][0], 1, 1, f);
        fwrite(&roomList[i][1], 1, 1, f);
        fwrite(&roomList[i][2], 1, 1, f);
        fwrite(&roomList[i][3], 1, 1, f);
    }
    fclose(f);
    return 0;
}


int load_dungeon(char* path, int* version){
    char f_type[12];
    int f_version = *version;
    int f_size;
    FILE *f;

    f = fopen(path, "r");
    if (f == NULL){
        printf("No file at %s\n", path);
        return -1;
    }

    fread(f_type, 12, 1, f);
    fread(&f_version, 4, 1, f);
    fread(&f_size, 4, 1, f);
    fread(&pcX, 1, 1, f);
    fread(&pcY, 1, 1, f);

    f_version = be32toh(f_version);
    f_size = be32toh(f_size);

    for (int i = 0; i < 21; i++){
        for(int j = 0; j < 80; j++){
            fread(&board[i][j].density, 1, 1, f);
        }
    }

    int bytes = 1702;
    int e;
    for (e = 0; bytes < f_size; e++){
        fread(&roomList[e][0],1,1,f);
        fread(&roomList[e][1],1,1,f);
        fread(&roomList[e][2],1,1,f);
        fread(&roomList[e][3],1,1,f);
        bytes+=4;
    }
    num_rooms = e;
    fclose(f);

    for (int e = 0; e < num_rooms; e++) {
        for (int x = roomList[e][1]; x < roomList[e][1] + roomList[e][3]; x++) {
            for (int y = roomList[e][0]; y < roomList[e][0] + roomList[e][2]; y++) {
                if (board[x][y].density == 0) {
                    board[x][y].tile = ROOM;
                }
            }
        }
    }

    for (int i = 0; i < 21; i++){
        for (int j = 0; j < 80; j++){
            if ((board[i][j].density == 0) && (board[i][j].tile != '.')){
                board[i][j].tile = TUNNEL;
            }
            else if (board[i][j].density != 0){
                board[i][j].tile = ROCK;
            }
        }
    }
    print_dungeon();
    return 0;
}
