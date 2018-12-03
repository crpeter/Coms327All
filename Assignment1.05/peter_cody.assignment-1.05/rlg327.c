#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>


/* Very slow seed: 686846853 */

#include "dungeon.h"
#include "ncurses.h"

#include "pc.h"
#include "npc.h"
#include "move.h"

const char *victory =
  "\n                                       o\n"
  "                                      $\"\"$o\n"
  "                                     $\"  $$\n"
  "                                      $$$$\n"
  "                                      o \"$o\n"
  "                                     o\"  \"$\n"
  "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
  "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
  "\"oo   o o o o\n"
  "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
  "   \"o$$\"    o$$\n"
  "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
  "     o\"\"\n"
  "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
  "   o\"\n"
  "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
  "\"$$$  $\n"
  "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
  "\"\"      \"\"\" \"\n"
  "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
  "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
  "\"$$$$\n"
  "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
  "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
  "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
  "              $\"                                                 \"$\n"
  "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
  "$\"$\"$\"$\"$\"$\"$\"$\n"
  "                                   You win!\n\n";

const char *tombstone =
  "\n\n\n\n                /\"\"\"\"\"/\"\"\"\"\"\"\".\n"
  "               /     /         \\             __\n"
  "              /     /           \\            ||\n"
  "             /____ /   Rest in   \\           ||\n"
  "            |     |    Pieces     |          ||\n"
  "            |     |               |          ||\n"
  "            |     |   A. Luser    |          ||\n"
  "            |     |               |          ||\n"
  "            |     |     * *   * * |         _||_\n"
  "            |     |     *\\/* *\\/* |        | TT |\n"
  "            |     |     *_\\_  /   ...\"\"\"\"\"\"| |"
  "| |.\"\"....\"\"\"\"\"\"\"\".\"\"\n"
  "            |     |         \\/..\"\"\"\"\"...\"\"\""
  "\\ || /.\"\"\".......\"\"\"\"...\n"
  "            |     |....\"\"\"\"\"\"\"........\"\"\"\"\""
  "\"^^^^\".......\"\"\"\"\"\"\"\"..\"\n"
  "            |......\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"......"
  "..\"\"\"\"\"....\"\"\"\"\"..\"\"...\"\"\".\n\n"
  "            You're dead.  Better luck in the next life.\n\n\n";

void usage(char *name)
{
  fprintf(stderr,
          "Usage: %s [-r|--rand <seed>] [-l|--load [<file>]]\n"
          "          [-s|--save [<file>]] [-i|--image <pgm file>]\n"
          "          [-n|--nummon <count>]",
          name);

  exit(-1);
}

// declare and implement show menu
void show_menu(dungeon_t *d);
void show_menu(dungeon_t *d) {
    // Collect the mons
    character_t *monsters[100];
    int index = 0;
    for (int i = 0; i < DUNGEON_X; i++) {
        for (int j = 0; j < DUNGEON_Y; j++) {
            if (charxy(i, j)) {
                monsters[index] = charxy(i, j);
                index++;
            }
        }
    }
    
    // Start Menu; number of lines and keep track of index to display correct sequence of mon's
    mvprintw(0,0, "Press Esc to close Up/Down to scroll");
    int start_index = 0;
    int num_lines = 6;
    WINDOW *wnd = newwin(num_lines + 2, 35, 2, 0);
    int c = 0;
    do {
        if (c == KEY_UP) {
            start_index--;
        }
        if (c == KEY_DOWN) {
            start_index++;
        }
        if (start_index >= index - num_lines + 1) {
            start_index = index - num_lines + 1;
        }
        if (start_index < 0){
            start_index = 0;
        }
        clrtoeol();
        mvwprintw(wnd, 0, 1, "_______________________________");
        /* save the fancy stuff for another time */
//        mvwprintw(wnd, 1, 1, "| mon list                    |");
//        mvwprintw(wnd, 2, 1, "-------------------------------");
        for (int i = start_index; i < index; i++) {
            int dist_x, dist_y;
            if (d->pc.position[dim_x] < monsters[i]->position[dim_x]) {
                dist_x = monsters[i]->position[dim_x] - d->pc.position[dim_x];
            } else {
                dist_x = d->pc.position[dim_x] - monsters[i]->position[dim_x];
            }
            if (d->pc.position[dim_y] < monsters[i]->position[dim_y]) {
                dist_y = monsters[i]->position[dim_y] - d->pc.position[dim_y];
            } else {
                dist_y = d->pc.position[dim_y] - monsters[i]->position[dim_y];
            }
            //print distances
            char east_west[] = "West";
            char north_south[] = "North";
            if (monsters[i]->position[dim_x] > d->pc.position[dim_x]) {
                strcpy(east_west, "East");
            }
            if (monsters[i]->position[dim_y] > d->pc.position[dim_y]) {
                strcpy(north_south, "South");
            }
            mvwprintw(wnd, i+2 - start_index, 1, "|  %c is %2d %s and %2d %s  |",
                      monsters[i]->symbol, dist_x, east_west, dist_y, north_south);
        }
        mvwprintw(wnd, num_lines + 1, 1, "_______________________________");
        refresh();
        wrefresh(wnd);
        c = getch();
    } while (c != 27);
    move(0,0);
    clrtoeol();
    render_dungeon(d);
    refresh();
}

int main(int argc, char *argv[])
{
  dungeon_t d;
  time_t seed;
  struct timeval tv;
  uint32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;
  char *save_file;
  char *load_file;
  char *pgm_file;

  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
  do_seed = 1;
  save_file = load_file = NULL;
  d.max_monsters = MAX_MONSTERS;

  /* The project spec requires '--load' and '--save'.  It's common  *
   * interesting test dungeons for you.                             */
 
 if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 'n':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-nummon")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%hu", &d.max_monsters)) {
            usage(argv[0]);
          }
          break;
        case 'r':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-rand")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%lu", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        case 'l':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-load"))) {
            usage(argv[0]);
          }
          do_load = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            load_file = argv[++i];
          }
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll save to it.  If it is "seed", we'll save to    *
	     * <the current seed>.rlg327.  If it is "image", we'll  *
	     * save to <the current image>.rlg327.                  */
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    } else {
	      save_file = argv[i];
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
            /* There is another argument, and it's not a switch, so *
             * we'll treat it as a save file and try to load it.    */
            pgm_file = argv[++i];
          }
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to generate more than one dungeon *
     * per second, as opposed to time().           */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  if (!do_load && !do_image) {
    printf("Seed is %ld.\n", seed);
  }
  srand(seed);

  init_dungeon(&d);

  if (do_load) {
    read_dungeon(&d, load_file);
  } else if (do_image) {
    read_pgm(&d, pgm_file);
  } else {
    gen_dungeon(&d);
  }

  /* Ignoring PC position in saved dungeons.  Not a bug. */
  config_pc(&d);
  gen_monsters(&d);
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, true);

    while (pc_is_alive(&d) && dungeon_has_npcs(&d)) {

      int next_x, next_y = 0;
      render_dungeon(&d);

      int c = 0;
      c = getch();
      switch (c) {
          case 'm':
              //show_menu(&d);
              next_x = 0;
              next_y = 0;
              show_menu(&d);
              continue;
              break;
          case KEY_LEFT:
              next_x = -1;
              next_y = 0;
              break;
          case '4':
              next_x = -1;
              next_y = 0;
              break;
          case 'h':
              next_x = -1;
              next_y = 0;
              break;
          case KEY_UP:
              next_x = 0;
              next_y = -1;
              break;
          case '8':
              next_x = 0;
              next_y = -1;
              break;
          case 'k':
              next_x = 0;
              next_y = -1;
              break;
          case KEY_DOWN:
              next_x = 0;
              next_y = 1;
              break;
          case '2':
              next_x = 0;
              next_y = 1;
              break;
          case 'j':
              next_x = 0;
              next_y = 1;
              break;
          case KEY_RIGHT:
              next_x = 1;
              next_y = 0;
              break;
          case '6':
              next_x = 1;
              next_y = 0;
              break;
          case 'l':
              next_x = 1;
              next_y = 0;
              break;
          case 62:
              next_x = -50;
              next_y = -50;
              break;
          case 60:
              next_x = -50;
              next_y = 50;
              break;
          case '7':
              next_x = -1;
              next_y = -1;
              break;
          case 'y':
              next_x = -1;
              next_y = -1;
              break;
          case '9':
              next_x = 1;
              next_y = -1;
              break;
          case 'u':
              next_x = 1;
              next_y = -1;
              break;
          case '3':
              next_x = 1;
              next_y = 1;
              break;
          case 'n':
              next_x = 1;
              next_y = 1;
              break;
          case '1':
              next_x = -1;
              next_y = 1;
              break;
          case 'b':
              next_x = -1;
              next_y = 1;
              break;
          case 'Q':
              //quit game
              goto End_Game;
          case ' ':
              next_x = 0;
              next_y = 0;
              break;
          case '5':
              next_x = 0;
              next_y = 0;
              break;
          default:
              next_x = 2;
              next_y = 2;
              break;
      }
      refresh();
      do_moves(&d, next_x, next_y);
  }

    render_dungeon(&d);
    refresh();
    
End_Game:
  if (do_save) {
    if (do_save_seed) {
       /* 10 bytes for number, please dot, extention and null terminator. */
      save_file = malloc(18);
      sprintf(save_file, "%ld.rlg327", seed);
    }
    if (do_save_image) {
      if (!pgm_file) {
    fprintf(stderr, "No image file was loaded.  Using default.\n");
    do_save_image = 0;
      } else {
    /* Extension of 3 characters longer than image extension + null. */
    save_file = malloc(strlen(pgm_file) + 4);
    strcpy(save_file, pgm_file);
    strcpy(strchr(save_file, '.') + 1, "rlg327");
      }
    }
    write_dungeon(&d, save_file);

    if (do_save_seed || do_save_image) {
      free(save_file);
    }
  }

    move(0,0);
  printw("%s", pc_is_alive(&d) ? victory : tombstone);
  printw("You defended your life in the face of %u deadly beasts.\n"
         "You avenged the cruel and untimely murders of %u "
         "peaceful dungeon residents.\n",
         d.pc.kills[kill_direct], d.pc.kills[kill_avenged]);
    refresh();

  pc_delete(d.pc.pc);

  delete_dungeon(&d);

  return 0;
}
