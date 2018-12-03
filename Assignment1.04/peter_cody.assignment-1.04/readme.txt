##NEW FEATURES
##
#

Added to Dijikstra_Monster.c

Functions:
	movePlayerRandom(): this function makes a random number between 0 and 7 inclusive and then has the player move in one of the 8 directions if it is a valid move.

	moveMonster(): Function to move monster
	move_monster_help(): helper function to move a monster to the next tile depending on if the mon can tunnel or not. Using the distance gradients.

##PREVIOUS FEATURES
##
#

Dijikstra_Monster.c

The program does the following:

1. Use RWDungeon.c to create a 2d array (board) of structs that have info data members as follows: tile of type char, density (int), nonTunnelNPC (distance to player from tile for an npc that cannot tunnel), tunnelNPC (distance to player from tile for npc that can tunnel), x and y (int representing coordinates of tile).
2. Use prof.'s Fibonacci heap to implement Dijikstra's algorithm to find the distance from tiles that are reachable for non tunneling monsters to the player.
3. Use Fibonacci heap to implement Dijikstra to find distance to player from all tiles on the board as they are reachable by tunneling npc's.
4. Print the dungeon and the two different distance maps.

Functions:

display_distance_map(bool tunnel):
	Takes a boolean value tunnel that indicates if the method should print distance map for a monster 	that can tunnel or a monster that can't.

	If the distance for a tile is greater than 9 then the first character of the number as a string 	needs to be removed.

create_fibonacci_heap(heap_node[][] board_nodes, heap queue);
	Takes a 2d array of heap nodes that each represents one tile on the game board, and a heap. It 
	then fills the 2d array of heap nodes with heap nodes returned from the heap_insert method.

create_distance_maps():
	Creates two distance maps:
	Sets the tunnel and non tunnel distance to 0 on the tile where the player is located.
	1. Implement Dijiktras also different for non tunneling npc because you check all the neighbors of 	the current node and if one isn't a wall or room then nothing is done with that board tile.

Dungeon.c

The program does the following:

1. Generate the game board (dungeon) of size 80x21.
2. Generate at least 5 randomly generated rooms.
3. Make paths to fully connect all rooms in the dungeon.
4. Load and interpret file containing dungeons following format.
5. Save file to disk in format.

Functions:

load_dungeon():
	This functions takes a path and load the dungeon from it based on the file saving format if it exists in memory.

save_dungeon():
	Takes a path and creates a dungeon and then saves it to disk at specified path.

createRooms():
	This function generates at least 5 rooms by making pseudo random numbers for x position, y position, width and height of the room. If placing the room in the board does not conflict with other rooms then it is placed else a new room is generated.

addHallways():
	This function has a x reference point and a y reference point. All rooms on the board use the tunnelToPoint() function to add a hallway to the reference point. This ensures that all rooms in the dungeon are connected. This method many times created cycles between room which will improve playability.

tunnelToPoint():
	This function creates hallways between two points on the board. A board tile will be skipped if it has a '.'.