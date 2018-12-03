#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/* Very slow seed: 686846853 */

#include "dungeon.h"
#include "pc.h"
#include "npc.h"
#include "move.h"
#include "io.h"

#include <iostream>
#include <fstream>
#include <string>


//void print_mon(*string mon[][], int i);
using namespace std;

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

int main(int argc, char *argv[])
{
  dungeon d;
  time_t seed;
  struct timeval tv;
  int32_t i;
  uint32_t do_load, do_save, do_seed, do_image, do_save_seed, do_save_image;
  uint32_t long_arg;

  /* Quiet a false positive from valgrind. */
  memset(&d, 0, sizeof (d));
  
  /* Default behavior: Seed with the time, generate a new dungeon, *
   * and don't write to disk.                                      */
  do_load = do_save = do_image = do_save_seed = do_save_image = 0;
    do_load++;
  do_seed = 1;
  //save_file =
  d.max_monsters = MAX_MONSTERS;
 
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
          break;
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-save"))) {
            usage(argv[0]);
          }
          do_save = 1;
          if ((argc > i + 1) && argv[i + 1][0] != '-') {
	    if (!strcmp(argv[++i], "seed")) {
	      do_save_seed = 1;
	      do_save_image = 0;
	    } else if (!strcmp(argv[i], "image")) {
	      do_save_image = 1;
	      do_save_seed = 0;
	    }
          }
          break;
        case 'i':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-image"))) {
            usage(argv[0]);
          }
          do_image = 1;
          
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
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  srand(seed);
    
    uint8_t in_monster = 0;
    uint8_t in_desc = 0;
    string __mon_atrri__[250][2];
    
    string line;
    
    char *home;
    size_t len;
    char *filename;
    if (!(home = getenv("HOME"))) {
        fprintf(stderr, "\"HOME\" is undefined.  Using working directory.\n");
        home = (char *) ".";
    }
    
    len = (strlen(home) + strlen(".rlg327") + strlen("monster_desc.txt") +
           1 /* The NULL terminator */                                 +
           2 /* The slashes */);
    
    filename = (char *) malloc(len * sizeof (*filename));
    sprintf(filename, "%s/%s/%s", home, ".rlg327", "monster_desc.txt");
    ifstream myfile(filename);
    uint8_t index_attri = 0;
    uint8_t first_time = 0;
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            if (first_time == 0) {
                if (line.substr(0, 28).compare("RLG327 MONSTER DESCRIPTION 1") == 0) {
                    first_time = 1;
                }
            }
            string desc = "";
            if (line.substr(0, 13).compare("BEGIN MONSTER") == 0) {
                if (first_time != 1) { return -1; }
                in_monster = 1;
                continue;
            } else if (line.substr(0, 3).compare("END") == 0) {
                in_monster = 0;
                __mon_atrri__[index_attri++][0] = "_";
                continue;
            } else if (line.substr(0, 4).compare("DESC") == 0) {
                in_desc = 1;
                __mon_atrri__[index_attri][0] = "DESC";
                continue;
            }
            if (in_monster == 1) {
                if (in_desc) {
                    if (line.substr(0, 1).compare(".") == 0) {
                        in_desc = 0;
                        index_attri += 1;
                        continue;
                    } else {
                        if (__mon_atrri__[index_attri][1].empty()) {
                            __mon_atrri__[index_attri][1] = ' ';
                        }
                        __mon_atrri__[index_attri][1] += line;
                    }
                } else {
                    int pos = line.find(' ');
                    if (pos != -1) {
                        string key = line.substr(0, pos);
                        string values = line.substr(pos+1, line.length());

                        __mon_atrri__[index_attri][0] = key;
                        __mon_atrri__[index_attri][1] = values;
                    }
                    index_attri += 1;
                }
            }
        }
        int mon_start, mon_end;
        enum {
            NAME, DESC, SYMB, COLOR, SPEED, ABIL, HP, DAM, RRTY
        };
        string ordered_values[9];
        int i = 0;
        while (i < 250) {
            if (__mon_atrri__[i][0].compare("_") == 0 || i == 0) {

                if (i == 0) {
                    mon_start = i;
                } else {
                    mon_start = i+1;
                }
                mon_end = mon_start + 9;
                if (__mon_atrri__[mon_end][0].compare("_") == 0) {
                    while (mon_start <= mon_end) {
                        if (__mon_atrri__[mon_start][0].compare("HP") == 0 || __mon_atrri__[mon_start][0].compare("DAM") == 0 ||
                            __mon_atrri__[mon_start][0].compare("SPEED") == 0)
                        {
                            int plus_index, d_index;
                            plus_index = d_index = 0;
                            string first, second, third;
                            first.clear(); second.clear(); third.clear();
                            for (uint32_t x = 0; x < __mon_atrri__[mon_start][1].size(); x++) {
                                if (__mon_atrri__[mon_start][1].at(x) == '+') {
                                    plus_index = x;
                                    first = __mon_atrri__[mon_start][1].substr(0, x);
                                }
                                if (__mon_atrri__[mon_start][1].at(x) == 'd') {
                                    d_index = x;
                                }
                            }
                            while (plus_index+1 < d_index) {
                                second += __mon_atrri__[mon_start][1].at(plus_index + 1);
                                plus_index++;
                            }
                            third = __mon_atrri__[mon_start][1].substr(d_index+1, __mon_atrri__[mon_start][1].length());
                            int fi, se, thi;
                            fi = stoi(first);
                            se = stoi(second);
                            thi = stoi(third);
                            double min = fi+se;
                            double max = fi+(se*thi);
//                            cout << __mon_atrri__[mon_start][1];
//                            cout << " [min: " << fi+se << ", max: " << fi+(se*thi) << ", dice: " << se << ", sides: " << thi << ", expected: " << ((max-min)/2) + min << "]";
//                            cout << '\n';
                            string current = __mon_atrri__[mon_start][1];
                            current += " [min: "; current += to_string(fi+se); current += ", max: "; current += to_string(fi+(se*thi)); current += ", dice: "; current += to_string(se); current += ", sides: "; current += to_string(thi); current += ", expected: "; current += to_string(((max-min)/2) + min); current += "]";
                            if (__mon_atrri__[mon_start][0].compare("HP") == 0) {
                                ordered_values[HP].clear();
                                ordered_values[HP] = current;
                            } else if (__mon_atrri__[mon_start][0].compare("DAM") == 0) {
                                ordered_values[DAM].clear();
                                ordered_values[DAM] = current;
                            } else if (__mon_atrri__[mon_start][0].compare("SPEED") == 0) {
                                ordered_values[SPEED].clear();
                                ordered_values[SPEED] = current;
                            }
                            first.clear();
                            second.clear();
                            third.clear();
                            current.clear();
                            mon_start++;
                        } else {
                            //cout << __mon_atrri__[mon_start][1] << '\n';
                            string current = __mon_atrri__[mon_start][1];
                            if (__mon_atrri__[mon_start][0].compare("NAME") == 0) {
                                //ordered_values[NAME].clear();
                                ordered_values[NAME] = current;
                            } else if (__mon_atrri__[mon_start][0].compare("DESC") == 0) {
                                ordered_values[DESC].clear();
                                ordered_values[DESC] = current;
                            } else if (__mon_atrri__[mon_start][0].compare("SYMB") == 0) {
                                ordered_values[SYMB].clear();
                                ordered_values[SYMB] = current;
                            } else if (__mon_atrri__[mon_start][0].compare("COLOR") == 0) {
                                ordered_values[COLOR].clear();
                                ordered_values[COLOR] = current;
                            } else if (__mon_atrri__[mon_start][0].compare("ABIL") == 0) {
                                ordered_values[ABIL].clear();
                                ordered_values[ABIL] = current;
                            } else if (__mon_atrri__[mon_start][0].compare("RRTY") == 0) {
                                ordered_values[RRTY].clear();
                                ordered_values[RRTY] = current;
                            }
                            mon_start++;
                        }
                    }
                    cout << '\n';
                    uint8_t duplicate_by_the_pigeon_in_the_whole_hole = 0;
                    for (int k = 0; k < 9; k++) {
                        if (ordered_values[k].compare("") == 0) {
                            duplicate_by_the_pigeon_in_the_whole_hole = 1;
                        }
                    }
                    if (duplicate_by_the_pigeon_in_the_whole_hole == 0) {
                        for (int k = 0; k < 9; k++) {
                            cout << ordered_values[k] << '\n';
                        }
                        cout << '\n';
                    }
                    ordered_values[HP].clear();
                    ordered_values[DAM].clear();
                    ordered_values[SPEED].clear();
                    ordered_values[NAME].clear();
                    ordered_values[DESC].clear();
                    ordered_values[SYMB].clear();
                    ordered_values[COLOR].clear();
                    ordered_values[ABIL].clear();
                    ordered_values[RRTY].clear();
                }
            }
            i += 1;
        }
        myfile.close();
    } else cout << "Unable to open file";
//End:
    
  return 0;
}
