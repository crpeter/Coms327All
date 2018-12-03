//
//  KnightsTour.c
//  
//
//  Created by Cody Peter on 8/29/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int N = 5;
int total = 0;

bool isPossibleMove(int x, int y, int board[N][N])
{
    if (board[x][y] == -1 && y >= 0 && y < N && x >= 0 && x < N) {
        return true;
    } else {
        return false;
    }
}

void printSolution(int solution[N][N], int ref[N][N])
{
    printf("\n");
    int help = 1;
    while (help <= 25) {
        for (int x = 0; x < N; x++)
        {
            for (int y = 0; y < N; y++)
            {
                if (solution[x][y] == help) {
                    printf("%d", ref[x][y]);
                    help++;
                    if (help <= 25) {
                        printf(",");
                    }
                }
            }
        }
    }
}


void findTours(int visited[N][N], int currentX, int currentY, int index, int ref[N][N]) {
    
    visited[currentX][currentY] = index;
    
    if (index >= N*N) {
        printSolution(visited, ref);
        visited[currentX][currentY] = -1;
        total += 1;
        return;
    }
    
    int x[8] = {  2, 1, -1, -2, -2, -1,  1,  2 };
    int y[8] = {  1, 2,  2,  1, -1, -2, -2, -1 };
    
    for (int i = 0; i < 8; i++)
    {
        int testX = currentX + x[i];
        int testY = currentY + y[i];
        
        if (isPossibleMove(testX, testY, visited))
        {
            findTours(visited, testX, testY, index + 1, ref);
        }
    }
    
    visited[currentX][currentY] = -1;
}

int main()
{
    int visited[N][N];
    for (int x = 0; x < N; x++)
    {
        for (int y = 0; y < N; y++)
        {
            visited[x][y] = -1;
        }
    }
    int pos = 1;
    int ref[N][N];
    int index = 1;
    for (int x = 0; x < N; x++)
    {
        for (int y = 0; y < N; y++)
        {
            ref[x][y] = index;
            index++;
        }
    }
    for (int x = 0; x < N; x++) {
        for (int y = 0; y < N; y++) {
            findTours(visited, x, y, pos, ref);
        }
    }
    
    
    return 0;
}
