#ifndef PC_H
# define PC_H

# include <stdint.h>
# include <stdbool.h>

# include "dims.h"

typedef struct dungeon dungeon_t;

typedef struct pc {
} pc_t;

void pc_delete(pc_t *pc);
uint32_t pc_is_alive(dungeon_t *d);
void config_pc(dungeon_t *d);
bool pc_next_pos(dungeon_t *d, pair_t dir, int x_change, int y_change);
void place_pc(dungeon_t *d);
uint32_t pc_in_room(dungeon_t *d, uint32_t room);

#endif
