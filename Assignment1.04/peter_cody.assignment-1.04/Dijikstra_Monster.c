#include "RWDungeon.h"
#include "fibheap.h"

void move_monster_help(int mon_index, mon_struct *mon);

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
                board[i][j].mon.type = 0;
                board[i][j].mon.speed = 10;
                board[i][j].mon.alive = true;
                board[i][j].mon.x = j;
                board[i][j].mon.y = i;
                pcX = j;
                pcY = i;
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

void movePlayerRandom() {
    bool done = false;
    while (!done) {
        int direction = rand_range(0, 8);
        printf("%d", direction);
        switch (direction) {
            case 0:
                if (board[pcY-1][pcX].density == 0) {
                    board[pcY-1][pcX].mon.type = 0;
                    board[pcY][pcX].mon.type = -1;
                    pcY -= 1;
                    done = true;
                }
                break;
            case 1:
                if (board[pcY-1][pcX-1].density == 0) {
                    board[pcY-1][pcX-1].mon.type = 0;
                    board[pcY][pcX].mon.type = -1;
                    pcY -= 1;
                    pcX -= 1;
                    done = true;
                }
                break;
            case 2:
                if (board[pcY-1][pcX+1].density == 0) {
                    board[pcY-1][pcX+1].mon.type = 0;
                    board[pcY][pcX].mon.type = -1;
                    pcY -= 1;
                    pcX += 1;
                    done = true;
                }
                break;
            case 3:
                if (board[pcY][pcX+1].density == 0) {
                    board[pcY][pcX+1].mon.type = 0;
                    board[pcY][pcX].mon.type = -1;
                    pcX += 1;
                    done = true;
                }
                break;
            case 4:
                if (board[pcY][pcX-1].density == 0) {
                    board[pcY][pcX-1].mon.type = 0;
                    board[pcY][pcX].mon.type = -1;
                    pcX -= 1;
                    done = true;
                }
                break;
            case 5:
                if (board[pcY+1][pcX].density == 0) {
                    board[pcY+1][pcX].mon.type = 0;
                    board[pcY][pcX].mon.type = -1;
                    pcY += 1;
                    done = true;
                }
                break;
            case 6:
                if (board[pcY+1][pcX-1].density == 0) {
                    board[pcY+1][pcX-1].mon.type = 0;
                    board[pcY][pcX].mon.type = -1;
                    pcY += 1;
                    pcX -= 1;
                    done = true;
                }
                break;
            case 7:
                if (board[pcY+1][pcX+1].density == 0) {
                    board[pcY+1][pcX+1].mon.type = 0;
                    board[pcY][pcX].mon.type = -1;
                    pcY += 1;
                    pcX += 1;
                    done = true;
                }
                break;
            default:
                break;
        }
    }
}

void moveMonster(mon_struct *mon) {
    for (int i = 0; i < sizeof(monster_coords)/sizeof(monster_coords[0]); i++) {
        if (monster_coords[i][0] == -1) {
            break;
        } else {
            move_monster_help(i, mon);
        }
    }
}

void move_monster_help(int mon_index, mon_struct *mon) {
    int x = monster_coords[mon_index][0];
    int y = monster_coords[mon_index][1];
    int best_weight = 255;
    int bestX;
    int bestY;
    switch (board[y][x].mon.type) {
        case 1:
            if (board[y-1][x-1].nonTunnelNPC < best_weight) {
                best_weight = board[y-1][x-1].nonTunnelNPC;
                bestX = x-1;
                bestY = y-1;
            }
            if (board[y-1][x].nonTunnelNPC < best_weight) {
                best_weight = board[y-1][x].nonTunnelNPC;
                bestX = x;
                bestY = y-1;
            }
            if (board[y-1][x+1].nonTunnelNPC < best_weight) {
                best_weight = board[y-1][x+1].nonTunnelNPC;
                bestX = x+1;
                bestY = y-1;
            }
            if (board[y][x-1].nonTunnelNPC < best_weight) {
                best_weight = board[y][x-1].nonTunnelNPC;
                bestX = x-1;
                bestY = y;
            }
            if (board[y][x+1].nonTunnelNPC < best_weight) {
                best_weight = board[y][x+1].nonTunnelNPC;
                bestX = x+1;
                bestY = y;
            }
            if (board[y+1][x-1].nonTunnelNPC < best_weight) {
                best_weight = board[y+1][x-1].nonTunnelNPC;
                bestX = x-1;
                bestY = y+1;
            }
            if (board[y+1][x].nonTunnelNPC < best_weight) {
                best_weight = board[y+1][x].nonTunnelNPC;
                bestX = x;
                bestY = y+1;
            }
            if (board[y+1][x+1].nonTunnelNPC < best_weight) {
                best_weight = board[y+1][x+1].nonTunnelNPC;
                bestX = x+1;
                bestY = y+1;
            }
            printf("x: %d, y: %d", x, y);
            printf("bestx: %d bestY: %d, best weight: %d\n", bestX, bestY, best_weight);
            board[y][x].mon.type = -1;
            board[bestY][bestX].mon.type = 1;
            if (bestY == pcY && bestX == pcX) {
                game_over = true;
            }
            monster_coords[mon_index][0] = bestX;
            monster_coords[mon_index][1] = bestY;
            break;
        case 2:
            best_weight = 255;
            if (board[y-1][x-1].tunnelNPC <= best_weight) {
                best_weight = board[y-1][x-1].tunnelNPC;
                bestX = x-1;
                bestY = y-1;
            }
            if (board[y-1][x].tunnelNPC <= best_weight) {
                best_weight = board[y-1][x].tunnelNPC;
                bestX = x;
                bestY = y-1;
            }
            if (board[y-1][x+1].tunnelNPC <= best_weight) {
                best_weight = board[y-1][x+1].tunnelNPC;
                bestX = x+1;
                bestY = y-1;
            }
            if (board[y][x-1].tunnelNPC <= best_weight) {
                best_weight = board[y][x-1].tunnelNPC;
                bestX = x-1;
                bestY = y;
            }
            if (board[y][x+1].tunnelNPC <= best_weight) {
                best_weight = board[y][x+1].tunnelNPC;
                bestX = x+1;
                bestY = y;
            }
            if (board[y+1][x-1].tunnelNPC <= best_weight) {
                best_weight = board[y+1][x-1].tunnelNPC;
                bestX = x-1;
                bestY = y+1;
            }
            if (board[y+1][x].tunnelNPC <= best_weight) {
                best_weight = board[y+1][x].tunnelNPC;
                bestX = x;
                bestY = y+1;
            }
            if (board[y+1][x+1].tunnelNPC <= best_weight) {
                best_weight = board[y+1][x+1].tunnelNPC;
                bestX = x+1;
                bestY = y+1;
            }
            printf("x: %d, y: %d", x, y);
            printf("bestx: %d bestY: %d, best weight: %d\n", bestX, bestY, best_weight);
            board[y][x].mon.type = -1;
            if (bestY == pcY && bestX == pcX) {
                game_over = true;
            }
            board[bestY][bestX].mon.type = 2;
            monster_coords[mon_index][0] = bestX;
            monster_coords[mon_index][1] = bestY;
            
            break;
    }
    printf("mon index %d\n", mon_index);
}

void add_mons() {
    int mon_index = 0;
    while (mon_index != 2) {
        int x = rand_range(20, 70);
        int y = rand_range(2, 19);
        if (board[y][x].density == 0) {
            if (!board[y][x].player && board[y][x].mon.type == -1) {
                board[y][x].mon.type = mon_index+1;
                board[y][x].mon.speed = rand_range(5, 20);
                board[y][x].mon.alive = true;
                board[y][x].mon.x = x;
                board[y][x].mon.y = y;
                monster_coords[mon_index][0] = x;
                monster_coords[mon_index][1] = y;
//                all_npc[done].x = x;
//                all_npc[done].y = y;
//                all_npc[done].speed = rand_range(5, 20);
//                all_npc[done].type = done;
//                all_npc[done].isPlayer = false;
//                all_npc[done].alive = true;
                mon_index += 1;
            }
        }
    }
}

int32_t sort_heap(const void* key, const void* with){
    return ((mon_struct*)key)->speed - ((mon_struct*)with)->speed;
}

int main(int argc, char* argv[]) {
    for (int i = 0; i < sizeof(monster_coords)/sizeof(monster_coords[0]); i++) {
        monster_coords[i][0] = -1;
        monster_coords[i][1] = -1;
    }
    srand(time(NULL));
    heap_t priority_queue;
    heap_init(&priority_queue, sort_heap, NULL);
    
    main_dungeon(argc, argv);
    create_distance_maps();
    
    heap_insert(&priority_queue, &board[pcY][pcX].mon);
    
    add_mons();
    print_dungeon();

    for (int i = 0; i < 2; i++){
        heap_insert(&priority_queue, &board[monster_coords[i][1]][monster_coords[i][0]]);
    }

    while (!game_over) {
        fflush(stdout);
        system("clear");

        mon_struct* min = (mon_struct*) heap_peek_min(&priority_queue);
        printf("min type: %d\n", min->type);
        if (min->type == 0) {
            movePlayerRandom();
            heap_remove_min(&priority_queue);
            heap_insert(&priority_queue, min);
        } else {
            moveMonster(min);
            heap_remove_min(&priority_queue);
            heap_insert(&priority_queue, min);
        }
        print_dungeon();
        usleep(250000);
        //help = 1;
    }
    printf("\nGAME OVER\n");

    //printf("pcx: %d, pcy: %d\n", pcX, pcY);
    //display_distance_map(false);
    //display_distance_map(true);
    return 1;
}
