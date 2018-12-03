Dungeon.c

The program does the following:

1. Generate the game board (dungeon) of size 80x21.
2. Generate at least 5 randomly generated rooms.
3. Make paths to fully connect all rooms in the dungeon.

Functions:

createRooms():
	This function generates at least 5 rooms by making pseudo random numbers for x position, y position, width and height of the room. If placing the room in the board does not conflict with other rooms then it is placed else a new room is generated.

addHallways():
	This function has a x reference point and a y reference point. All rooms on the board use the tunnelToPoint() function to add a hallway to the reference point. This ensures that all rooms in the dungeon are connected. This method many times created cycles between room which will improve playability.

tunnelToPoint():
	This function creates hallways between two points on the board. A board tile will be skipped if it has a '.'.