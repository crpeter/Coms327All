#include "RWDungeon.h"
#include "fibheap.h"


static int32_t distance_non_tunnel(const void *key, const void *with){
    return ((tile_struct *) key)->nonTunnelNPC - ((tile_struct *) with)->nonTunnelNPC;
}

static int32_t distance_tunnel(const void *key, const void *with){
    return ((tile_struct *) key)->tunnelNPC - ((tile_struct *) with)->tunnelNPC;
}

void create_fibonacci_heap(heap_node_t *board_nodes[DUNGEON_Y][DUNGEON_X], heap_t *queue){
    int i, j;
    for (i = 0; i < DUNGEON_Y; i++){
        for (j = 0; j < DUNGEON_X; j++) {
            board_nodes[i][j] = heap_insert(queue, &board[i][j]);
        }
    }
}

void  create_distance_maps() {

    heap_t priority_queue;
    heap_node_t *board_nodes[DUNGEON_Y][DUNGEON_X];
    
    bool found_char = false;
    for(int i = 0; i < DUNGEON_Y; i++){
        for(int j = 0; j < DUNGEON_X; j++){
            if(board[i][j].density == 0){
                found_char = true;
                board[i][j].tunnelNPC = 0;
                board[i][j].nonTunnelNPC = 0;
                board[i][j].tile = '@';
            }
            if (found_char) { break; }
        }
        if (found_char) { break; }
    }

    //map for monsters that cant tunnel
    heap_init(&priority_queue, distance_non_tunnel, NULL);
    create_fibonacci_heap(board_nodes, &priority_queue);
    while(priority_queue.size != 0) {
        tile_struct* peek_tile = (tile_struct*)heap_peek_min(&priority_queue);
        heap_remove_min(&priority_queue);
        int distance = ((peek_tile)->nonTunnelNPC) + 1;
        int x = (peek_tile)->x;
        int y = (peek_tile)->y;
        if (y - 1 >= 0) {
            if (board[x][y-1].density == 0){
                if (board[x][y-1].nonTunnelNPC > distance) {
                    board[x][y-1].nonTunnelNPC = distance;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x][y-1]);
                }
            }
        }
        if (x - 1 >= 0) {
            if (board[x-1][y].density == 0) {
                if (board[x-1][y].nonTunnelNPC > distance) {
                    board[x-1][y].nonTunnelNPC = distance;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x-1][y]);
                }
            }
        }
        if (y + 1 < DUNGEON_X) {
            if (board[x][y+1].density == 0) {
                if (board[x][y+1].nonTunnelNPC > distance) {
                    board[x][y+1].nonTunnelNPC = distance;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x][y+1]);
                }
            }
        }
        if (x + 1 < DUNGEON_Y) {
            if (board[x+1][y].density == 0) {
                if (board[x+1][y].nonTunnelNPC > distance) {
                    board[x+1][y].nonTunnelNPC = distance;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x+1][y]);
                }
            }
        }
        if (x + 1 < DUNGEON_Y && y + 1 < DUNGEON_X) {
            if (board[x+1][y+1].density == 0) {
                if (board[x+1][y+1].nonTunnelNPC > distance) {
                    board[x+1][y+1].nonTunnelNPC = distance;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x+1][y+1]);
                }
            }
        }
        if (x + 1 < DUNGEON_Y && y - 1 >= 0) {
            if (board[x+1][y-1].density == 0) {
                if (board[x+1][y-1].nonTunnelNPC > distance) {
                    board[x+1][y-1].nonTunnelNPC = distance;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x+1][y-1]);
                }
            }
        }
        if (x - 1 >= 0 && y + 1 < DUNGEON_X) {
            if (board[x-1][y+1].density == 0){
                if (board[x-1][y+1].nonTunnelNPC > distance) {
                    board[x-1][y+1].nonTunnelNPC = distance;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x-1][y+1]);
                }
            }
        }
        if (x - 1 >= 0 && y - 1 >= 0) {
            if (board[x-1][y-1].density == 0) {
                if (board[x-1][y-1].nonTunnelNPC > distance) {
                    board[x-1][y-1].nonTunnelNPC = distance;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x-1][y-1]);
                }
            }
        }
    }

    //map for monsters that can tunnel
    heap_init(&priority_queue, distance_tunnel, NULL);
    create_fibonacci_heap(board_nodes, &priority_queue);
    while(priority_queue.size != 0){
        tile_struct* peek_tile = (tile_struct*)heap_peek_min(&priority_queue);
        heap_remove_min(&priority_queue);
        int x = (peek_tile)->x;
        int y = (peek_tile)->y;
        int distance = (peek_tile)->tunnelNPC;
        int distance_weighted;
        
        if (y - 1 >= 0) {
            if (board[x][y-1].density < 255){
                if (board[x][y-1].density == 0) {
                    distance_weighted = distance + 1;
                } else {
                    distance_weighted = distance + board[x][y-1].density / 85;
                }
                if (board[x][y-1].tunnelNPC > distance_weighted){
                    board[x][y-1].tunnelNPC = distance_weighted;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x][y-1]);
                }
            }
        }
        if (x - 1 >= 0) {
            if (board[x-1][y].density < 255){
                if (board[x-1][y].density == 0) {
                    distance_weighted = distance + 1;
                } else {
                    distance_weighted = distance + board[x-1][y].density / 85;
                }
                if (board[x-1][y].tunnelNPC > distance_weighted) {
                    board[x-1][y].tunnelNPC = distance_weighted;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x-1][y]);
                }
            }
        }
        if (y + 1 < DUNGEON_X) {
            if (board[x][y+1].density < 255){
                if (board[x][y+1].density == 0) {
                    distance_weighted = distance + 1;
                } else {
                    distance_weighted = distance + board[x][y+1].density / 85;
                }
                if (board[x][y+1].tunnelNPC > distance_weighted){
                    board[x][y+1].tunnelNPC = distance_weighted;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x][y+1]);
                }
            }
        }
        if (x + 1 < DUNGEON_Y) {
            if (board[x+1][y].density < 255){
                if (board[x+1][y].density == 0) {
                    distance_weighted = distance + 1;
                } else {
                    distance_weighted = distance + board[x+1][y].density / 85;
                }
                if (board[x+1][y].tunnelNPC > distance_weighted){
                    board[x+1][y].tunnelNPC = distance_weighted;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x+1][y]);
                }
            }
        }
        if (x - 1 >= 0 && y + 1 < DUNGEON_X) {
            if (board[x-1][y+1].density < 255){
                if (board[x-1][y+1].density == 0) {
                    distance_weighted = distance + 1;
                } else {
                    distance_weighted = distance + board[x-1][y+1].density / 85;
                }
                if (board[x-1][y+1].tunnelNPC > distance_weighted){
                    board[x-1][y+1].tunnelNPC = distance_weighted;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x-1][y+1]);
                }
            }
        }
        if (x + 1 < DUNGEON_Y && y + 1 < DUNGEON_X) {
            if (board[x+1][y+1].density < 255){
                if (board[x+1][y+1].density == 0) {
                    distance_weighted = distance + 1;
                } else {
                    distance_weighted = distance + board[x+1][y+1].density / 85;
                }
                if (board[x+1][y+1].tunnelNPC > distance_weighted){
                    board[x+1][y+1].tunnelNPC = distance_weighted;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x+1][y+1]);
                }
            }
        }
        if (x - 1 >= 0 && y - 1 >= 0) {
            if (board[x-1][y-1].density < 255){
                if (board[x-1][y-1].density == 0) {
                    distance_weighted = distance + 1;
                } else {
                    distance_weighted = distance + board[x-1][y-1].density / 85;
                }
                if (board[x-1][y-1].tunnelNPC > distance_weighted){
                    board[x-1][y-1].tunnelNPC = distance_weighted;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x-1][y-1]);
                }
            }
        }
        if (x + 1 < DUNGEON_Y && y - 1 >= 0) {
            if (board[x+1][y-1].density < 255){
                if (board[x+1][y-1].density == 0) {
                    distance_weighted = distance + 1;
                } else {
                    distance_weighted = distance + board[x+1][y-1].density / 85;
                }
                if (board[x+1][y-1].tunnelNPC > distance_weighted){
                    board[x+1][y-1].tunnelNPC = distance_weighted;
                    heap_decrease_key_no_replace(&priority_queue, board_nodes[x+1][y-1]);
                }
            }
        }
    }
    heap_delete(&priority_queue);
}

void display_distance_map(bool tunnel){
    printf("\n");
    for(int i = 0; i < DUNGEON_Y; i++) {
        for (int j = 0; j < DUNGEON_X; j++) {
            int distance;
            if (!tunnel) {
                distance = board[i][j].nonTunnelNPC;
            } else {
                distance = board[i][j].tunnelNPC;
            }
            for (int x = 10; x <= 80; x+=10) {
                if (distance < x) {
                    distance -= (x - 10);
                    distance += 48;
                    break;
                }
                if (x == 80) {
                    distance = 32;
                }
            }
            printf("%c", distance);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    main_dungeon(argc, argv);
    create_distance_maps();
    print_dungeon();
    display_distance_map(false);
    display_distance_map(true);
    return 1;
}
