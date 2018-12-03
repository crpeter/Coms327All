#include <stdlib.h>

#include "utils.h"
#include "npc.h"
#include "dungeon.h"
#include "character.h"
#include "move.h"
#include "path.h"
#include "event.h"
#include "pc.h"
#include "descriptions.h"
#include "dice.h"

static uint32_t max_monster_cells(dungeon *d)
{
  uint32_t i;
  uint32_t sum;

  for (i = sum = 0; i < d->num_rooms; i++) {
    if (!pc_in_room(d, i)) {
      sum += d->rooms[i].size[dim_y] * d->rooms[i].size[dim_x];
    }
  }

  return sum;
}

void create_monsters(dungeon *d) {
  npc *m;
  for (auto i = d->monster_descriptions.begin(); i != d->monster_descriptions.end(); ++i) {
    m = new npc(i->name, i->description,
                i->abilities,
                i->hitpoints.roll(), i->damage,
                i->rarity);
    m->symbol = i->get_symbol();
    m->speed = i->speed.roll();
    m->color = i->color.at(0);
    m->alive = 1;
    d->npc_list.push_back(*m);
  }
}

void gen_monsters(dungeon *d)
{
  npc *m;
  uint32_t room;
  pair_t p;
  uint32_t num_cells;
  //memset(d->character_map, 0, sizeof (d->character_map));
  
  int placed_monsters = 0;
  int max_monsters = 0;
  for (auto i = d->npc_list.begin(); i != d->npc_list.end(); ++i) {
    i->placed = false;
    if (i->alive) {
      max_monsters++;
    }
  }
  int num_monsters = rand_range(1, max_monsters-1);
  d->num_monsters = num_monsters;
  while (placed_monsters < num_monsters) {
    num_cells = max_monster_cells(d);
    int mon_index;
    int rand_num;
    do {
      mon_index = rand_range(1, d->npc_list.size()-1);
      rand_num = rand_range(0, 99);
    } while (!d->npc_list.at(mon_index).alive ||
             d->npc_list.at(mon_index).placed ||
             rand_num >= d->npc_list.at(mon_index).rarity);
    m = &d->npc_list.at(mon_index);
    m->placed = true;
    do {
      room = rand_range(1, d->num_rooms - 1);
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                            (d->rooms[room].position[dim_y] +
                             d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                            (d->rooms[room].position[dim_x] +
                             d->rooms[room].size[dim_x] - 1));
    } while (d->character_map[p[dim_y]][p[dim_x]]);
    m->position[dim_y] = p[dim_y];
    m->position[dim_x] = p[dim_x];
    m->speed = rand_range(5, 20);
    m->alive = 1;
    m->sequence_number = ++d->character_sequence_number;
    m->characteristics = rand() & 0x0000000f;
    m->have_seen_pc = 0;
    m->kills[kill_direct] = m->kills[kill_avenged] = 0;
    
    d->character_map[p[dim_y]][p[dim_x]] = m;
    
    heap_insert(&d->events, new_event(d, event_characterurn, m, 0));
    placed_monsters++;
  }
}
//  int monsters_killed = 0;
//  for (auto i = d->monster_descriptions.begin(); i != d->monster_descriptions.end(); ++i) {
//    if (i->killed) {
//      monsters_killed++;
//    }
//  }
//  int num_monsters;
//  if (d->monster_descriptions.size()-1-monsters_killed <= 10) {
//    num_monsters = rand_range(0, d->monster_descriptions.size()-1-monsters_killed);
//  } else {
//    num_monsters = rand_range(10, d->monster_descriptions.size()-1-monsters_killed);
//  }
//  int placed_monsters = 0;
//  d->num_monsters = num_monsters;
//  //cout << "num monsters: [" << num_monsters << "], vector size:[" << d->monster_descriptions.size() << "]\n";
//  while (placed_monsters < num_monsters) {
//    int monster_index;
//    do {
//      monster_index = rand_range(0, num_monsters);
//    } while (d->monster_descriptions.at(monster_index).killed ||
//             contains_mon(d, d->monster_descriptions.at(monster_index)));
//    monster_description *i = &d->monster_descriptions.at(monster_index);
//    m = new npc(i->name, i->description,
//                i->abilities,
//                i->hitpoints.roll(), i->damage,
//                i->rarity);
//    m->symbol = i->get_symbol();
//    m->speed = i->speed.roll();
//    m->color = i->color.at(0);
//    cout << "name: " << m->name << "color: " << m->color << '\n';
//    do {
//      room = rand_range(1, d->num_rooms - 1);
//      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
//                            (d->rooms[room].position[dim_y] +
//                             d->rooms[room].size[dim_y] - 1));
//      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
//                            (d->rooms[room].position[dim_x] +
//                             d->rooms[room].size[dim_x] - 1));
//    } while (d->character_map[p[dim_y]][p[dim_x]]);
//    m->position[dim_y] = p[dim_y];
//    m->position[dim_x] = p[dim_x];
//    //cout << "found one: [" << m->symbol << "],[x=" << m->position[dim_x] << "],[y=" << m->position[dim_y] << "]\n";
//
//    d->character_map[p[dim_y]][p[dim_x]] = m;
//    m->speed = rand_range(5, 20);
//    m->alive = 1;
//    m->sequence_number = ++d->character_sequence_number;
//    m->characteristics = rand() & 0x0000000f;
//    /*    m->npc->characteristics = 0xf;*/
//    //m->symbol = symbol[m->characteristics];
//    m->have_seen_pc = 0;
//    m->kills[kill_direct] = m->kills[kill_avenged] = 0;
//
//    d->character_map[p[dim_y]][p[dim_x]] = m;
//
//    heap_insert(&d->events, new_event(d, event_characterurn, m, 0));
//    placed_monsters++;
//  }

void create_objects(dungeon *d) {
  object *temp;
  for (auto i = d->object_descriptions.begin(); i != d->object_descriptions.end(); ++i) {
    temp = new object(i->get_name(), i->get_description(),
                      i->get_type(), i->get_color(),
                      i->get_hit().roll(),
                      i->get_dodge().roll(),
                      i->get_defence().roll(),
                      i->get_weight().roll(),
                      i->get_speed().roll(),
                      i->get_attribute().roll(),
                      i->get_value().roll(),
                      i->get_damage(), false, i->get_rrty());
    temp->used = false;
    d->object_list.push_back(*temp);
  }
}

void gen_objects(dungeon *d) {
  object *temp;
  uint32_t room;
  pair_t p;
  
  int destroyed_objects = 0;
  for (auto i = d->object_list.begin(); i != d->object_list.end(); ++i)
  {
    if (i->artifact && i->used) {
      destroyed_objects++;
    }
  }
  int num_objects;
  if (d->object_descriptions.size() - destroyed_objects <= 10) {
    num_objects = rand_range(0, d->object_descriptions.size());
  } else {
    num_objects = rand_range(10, d->object_descriptions.size());
  }
  int num_objects_placed = 0;
  while (num_objects_placed < num_objects) {
    do {
      temp = &d->object_list.at(rand_range(0, d->object_list.size()-1));
    } while (temp->artifact && temp->used);
    cout << temp->get_name() << '\n';
    do {
      room = rand_range(1, d->num_rooms - 1);
      p[dim_y] = rand_range(d->rooms[room].position[dim_y],
                            (d->rooms[room].position[dim_y] +
                             d->rooms[room].size[dim_y] - 1));
      p[dim_x] = rand_range(d->rooms[room].position[dim_x],
                            (d->rooms[room].position[dim_x] +
                             d->rooms[room].size[dim_x] - 1));
    } while (d->object_map[p[dim_y]][p[dim_x]]);
    d->object_map[p[dim_y]][p[dim_x]] = temp;
    temp->position[dim_x] = p[dim_x];
    temp->position[dim_y] = p[dim_y];
    num_objects_placed++;
  }
}

void npc_next_pos_rand_tunnel(dungeon *d, npc *c, pair_t next)
{
  pair_t n;
  union {
    uint32_t i;
    uint8_t a[4];
  } r;

  do {
    n[dim_y] = next[dim_y];
    n[dim_x] = next[dim_x];
    r.i = rand();
    if (r.a[0] > 85 /* 255 / 3 */) {
      if (r.a[0] & 1) {
        n[dim_y]--;
      } else {
        n[dim_y]++;
      }
    }
    if (r.a[1] > 85 /* 255 / 3 */) {
      if (r.a[1] & 1) {
        n[dim_x]--;
      } else {
        n[dim_x]++;
      }
    }
  } while (mappair(n) == ter_wall_immutable);

  if (hardnesspair(n) <= 85) {
    if (hardnesspair(n)) {
      hardnesspair(n) = 0;
      mappair(n) = ter_floor_hall;

      /* Update distance maps because map has changed. */
      dijkstra(d);
      dijkstra_tunnel(d);
    }

    next[dim_x] = n[dim_x];
    next[dim_y] = n[dim_y];
  } else {
    hardnesspair(n) -= 85;
  }
}

void npc_next_pos_rand(dungeon *d, npc *c, pair_t next)
{
  pair_t n;
  union {
    uint32_t i;
    uint8_t a[4];
  } r;

  do {
    n[dim_y] = next[dim_y];
    n[dim_x] = next[dim_x];
    r.i = rand();
    if (r.a[0] > 85 /* 255 / 3 */) {
      if (r.a[0] & 1) {
        n[dim_y]--;
      } else {
        n[dim_y]++;
      }
    }
    if (r.a[1] > 85 /* 255 / 3 */) {
      if (r.a[1] & 1) {
        n[dim_x]--;
      } else {
        n[dim_x]++;
      }
    }
  } while (mappair(n) < ter_floor);

  next[dim_y] = n[dim_y];
  next[dim_x] = n[dim_x];
}

void npc_next_pos_line_of_sight(dungeon *d, npc *c, pair_t next)
{
  pair_t dir;

  dir[dim_y] = d->PC->position[dim_y] - c->position[dim_y];
  dir[dim_x] = d->PC->position[dim_x] - c->position[dim_x];
  if (dir[dim_y]) {
    dir[dim_y] /= abs(dir[dim_y]);
  }
  if (dir[dim_x]) {
    dir[dim_x] /= abs(dir[dim_x]);
  }

  if (mapxy(next[dim_x] + dir[dim_x],
            next[dim_y] + dir[dim_y]) >= ter_floor) {
    next[dim_x] += dir[dim_x];
    next[dim_y] += dir[dim_y];
  } else if (mapxy(next[dim_x] + dir[dim_x], next[dim_y]) >= ter_floor) {
    next[dim_x] += dir[dim_x];
  } else if (mapxy(next[dim_x], next[dim_y] + dir[dim_y]) >= ter_floor) {
    next[dim_y] += dir[dim_y];
  }
}

void npc_next_pos_line_of_sight_tunnel(dungeon *d,
                                       npc *c,
                                       pair_t next)
{
  pair_t dir;

  dir[dim_y] = d->PC->position[dim_y] - c->position[dim_y];
  dir[dim_x] = d->PC->position[dim_x] - c->position[dim_x];
  if (dir[dim_y]) {
    dir[dim_y] /= abs(dir[dim_y]);
  }
  if (dir[dim_x]) {
    dir[dim_x] /= abs(dir[dim_x]);
  }

  dir[dim_x] += next[dim_x];
  dir[dim_y] += next[dim_y];

  if (hardnesspair(dir) <= 85) {
    if (hardnesspair(dir)) {
      hardnesspair(dir) = 0;
      mappair(dir) = ter_floor_hall;

      /* Update distance maps because map has changed. */
      dijkstra(d);
      dijkstra_tunnel(d);
    }

    next[dim_x] = dir[dim_x];
    next[dim_y] = dir[dim_y];
  } else {
    hardnesspair(dir) -= 85;
  }
}

void npc_next_pos_gradient(dungeon *d, npc *c, pair_t next)
{
  /* Handles both tunneling and non-tunneling versions */
  pair_t min_next;
  uint16_t min_cost;
  if (c->characteristics & NPC_TUNNEL) {
    min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x]] +
                (d->hardness[next[dim_y] - 1][next[dim_x]] / 85));
    min_next[dim_x] = next[dim_x];
    min_next[dim_y] = next[dim_y] - 1;
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x]    ] +
         (d->hardness[next[dim_y] + 1][next[dim_x]] / 85)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x]] +
                  (d->hardness[next[dim_y] + 1][next[dim_x]] / 85));
      min_next[dim_x] = next[dim_x];
      min_next[dim_y] = next[dim_y] + 1;
    }
    if ((d->pc_tunnel[next[dim_y]    ][next[dim_x] + 1] +
         (d->hardness[next[dim_y]    ][next[dim_x] + 1] / 85)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y]][next[dim_x] + 1] +
                  (d->hardness[next[dim_y]][next[dim_x] + 1] / 85));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y];
    }
    if ((d->pc_tunnel[next[dim_y]    ][next[dim_x] - 1] +
         (d->hardness[next[dim_y]    ][next[dim_x] - 1] / 85)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y]][next[dim_x] - 1] +
                  (d->hardness[next[dim_y]][next[dim_x] - 1] / 85));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y];
    }
    if ((d->pc_tunnel[next[dim_y] - 1][next[dim_x] + 1] +
         (d->hardness[next[dim_y] - 1][next[dim_x] + 1] / 85)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x] + 1] +
                  (d->hardness[next[dim_y] - 1][next[dim_x] + 1] / 85));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y] - 1;
    }
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x] + 1] +
         (d->hardness[next[dim_y] + 1][next[dim_x] + 1] / 85)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x] + 1] +
                  (d->hardness[next[dim_y] + 1][next[dim_x] + 1] / 85));
      min_next[dim_x] = next[dim_x] + 1;
      min_next[dim_y] = next[dim_y] + 1;
    }
    if ((d->pc_tunnel[next[dim_y] - 1][next[dim_x] - 1] +
         (d->hardness[next[dim_y] - 1][next[dim_x] - 1] / 85)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] - 1][next[dim_x] - 1] +
                  (d->hardness[next[dim_y] - 1][next[dim_x] - 1] / 85));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y] - 1;
    }
    if ((d->pc_tunnel[next[dim_y] + 1][next[dim_x] - 1] +
         (d->hardness[next[dim_y] + 1][next[dim_x] - 1] / 85)) < min_cost) {
      min_cost = (d->pc_tunnel[next[dim_y] + 1][next[dim_x] - 1] +
                  (d->hardness[next[dim_y] + 1][next[dim_x] - 1] / 85));
      min_next[dim_x] = next[dim_x] - 1;
      min_next[dim_y] = next[dim_y] + 1;
    }
    if (hardnesspair(min_next) <= 85) {
      if (hardnesspair(min_next)) {
        hardnesspair(min_next) = 0;
        mappair(min_next) = ter_floor_hall;

        /* Update distance maps because map has changed. */
        dijkstra(d);
        dijkstra_tunnel(d);
      }

      next[dim_x] = min_next[dim_x];
      next[dim_y] = min_next[dim_y];
    } else {
      hardnesspair(min_next) -= 85;
    }
  } else {
    /* Make monsters prefer cardinal directions */
    if (d->pc_distance[next[dim_y] - 1][next[dim_x]    ] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x]    ] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      return;
    }
    if (d->pc_distance[next[dim_y]    ][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y]    ][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_x]--;
      return;
    }
    if (d->pc_distance[next[dim_y] - 1][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x] + 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      next[dim_x]++;
      return;
    }
    if (d->pc_distance[next[dim_y] - 1][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]--;
      next[dim_x]--;
      return;
    }
    if (d->pc_distance[next[dim_y] + 1][next[dim_x] - 1] <
        d->pc_distance[next[dim_y]][next[dim_x]]) {
      next[dim_y]++;
      next[dim_x]--;
      return;
    }
  }
}

static void npc_next_pos_00(dungeon *d, npc *c, pair_t next)
{
  /* not smart; not telepathic; not tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = d->PC->position[dim_y];
    c->pc_last_known_position[dim_x] = d->PC->position[dim_x];
    npc_next_pos_line_of_sight(d, c, next);
  } else {
    npc_next_pos_rand(d, c, next);
  }
}

static void npc_next_pos_01(dungeon *d, npc *c, pair_t next)
{
  /*     smart; not telepathic; not tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = d->PC->position[dim_y];
    c->pc_last_known_position[dim_x] = d->PC->position[dim_x];
    c->have_seen_pc = 1;
    npc_next_pos_line_of_sight(d, c, next);
  } else if (c->have_seen_pc) {
    npc_next_pos_line_of_sight(d, c, next);
  }

  if ((next[dim_x] == c->pc_last_known_position[dim_x]) &&
      (next[dim_y] == c->pc_last_known_position[dim_y])) {
    c->have_seen_pc = 0;
  }
}

static void npc_next_pos_02(dungeon *d, npc *c, pair_t next)
{
  /* not smart;     telepathic; not tunneling; not erratic */
  c->pc_last_known_position[dim_y] = d->PC->position[dim_y];
  c->pc_last_known_position[dim_x] = d->PC->position[dim_x];
  npc_next_pos_line_of_sight(d, c, next);
}

static void npc_next_pos_03(dungeon *d, npc *c, pair_t next)
{
  /*     smart;     telepathic; not tunneling; not erratic */
  npc_next_pos_gradient(d, c, next);
}

static void npc_next_pos_04(dungeon *d, npc *c, pair_t next)
{
  /* not smart; not telepathic;     tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = d->PC->position[dim_y];
    c->pc_last_known_position[dim_x] = d->PC->position[dim_x];
    npc_next_pos_line_of_sight(d, c, next);
  } else {
    npc_next_pos_rand_tunnel(d, c, next);
  }
}

static void npc_next_pos_05(dungeon *d, npc *c, pair_t next)
{
  /*     smart; not telepathic;     tunneling; not erratic */
  if (can_see(d, character_get_pos(c), character_get_pos(d->PC), 0, 0)) {
    c->pc_last_known_position[dim_y] = d->PC->position[dim_y];
    c->pc_last_known_position[dim_x] = d->PC->position[dim_x];
    c->have_seen_pc = 1;
    npc_next_pos_line_of_sight(d, c, next);
  } else if (c->have_seen_pc) {
    npc_next_pos_line_of_sight_tunnel(d, c, next);
  }

  if ((next[dim_x] == c->pc_last_known_position[dim_x]) &&
      (next[dim_y] == c->pc_last_known_position[dim_y])) {
    c->have_seen_pc = 0;
  }
}

static void npc_next_pos_06(dungeon *d, npc *c, pair_t next)
{
  /* not smart;     telepathic;     tunneling; not erratic */
  c->pc_last_known_position[dim_y] = d->PC->position[dim_y];
  c->pc_last_known_position[dim_x] = d->PC->position[dim_x];
  npc_next_pos_line_of_sight_tunnel(d, c, next);
}

static void npc_next_pos_07(dungeon *d, npc *c, pair_t next)
{
  /*     smart;     telepathic;     tunneling; not erratic */
  npc_next_pos_gradient(d, c, next);
}

static void npc_next_pos_08(dungeon *d, npc *c, pair_t next)
{
  /* not smart; not telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_00(d, c, next);
  }
}

static void npc_next_pos_09(dungeon *d, npc *c, pair_t next)
{
  /*     smart; not telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_01(d, c, next);
  }
}

static void npc_next_pos_0a(dungeon *d, npc *c, pair_t next)
{
  /* not smart;     telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_02(d, c, next);
  }
}

static void npc_next_pos_0b(dungeon *d, npc *c, pair_t next)
{
  /*     smart;     telepathic; not tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand(d, c, next);
  } else {
    npc_next_pos_03(d, c, next);
  }
}

static void npc_next_pos_0c(dungeon *d, npc *c, pair_t next)
{
  /* not smart; not telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_04(d, c, next);
  }
}

static void npc_next_pos_0d(dungeon *d, npc *c, pair_t next)
{
  /*     smart; not telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_05(d, c, next);
  }
}

static void npc_next_pos_0e(dungeon *d, npc *c, pair_t next)
{
  /* not smart;     telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_06(d, c, next);
  }
}

static void npc_next_pos_0f(dungeon *d, npc *c, pair_t next)
{
  /*     smart;     telepathic;     tunneling;     erratic */
  if (rand() & 1) {
    npc_next_pos_rand_tunnel(d, c, next);
  } else {
    npc_next_pos_07(d, c, next);
  }
}

void (*npc_move_func[])(dungeon *d, npc *c, pair_t next) = {
  /* We'll have one function for each combination of bits, so the *
   * order is based on binary counting through the NPC_* bits.    *
   * It could be very easy to mess this up, so be careful.  We'll *
   * name them according to their hex value.                      */
  npc_next_pos_00,
  npc_next_pos_01,
  npc_next_pos_02,
  npc_next_pos_03,
  npc_next_pos_04,
  npc_next_pos_05,
  npc_next_pos_06,
  npc_next_pos_07,
  npc_next_pos_08,
  npc_next_pos_09,
  npc_next_pos_0a,
  npc_next_pos_0b,
  npc_next_pos_0c,
  npc_next_pos_0d,
  npc_next_pos_0e,
  npc_next_pos_0f,
};

void npc_next_pos(dungeon *d, npc *c, pair_t next)
{
  next[dim_y] = c->position[dim_y];
  next[dim_x] = c->position[dim_x];

  npc_move_func[c->characteristics & 0x0000000f](d, c, next);
}

uint32_t dungeon_has_npcs(dungeon *d)
{
  return d->num_monsters;
}
