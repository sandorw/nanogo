/*
 * playout_policy.cpp
 *
 *  Created on: May 11, 2015
 *      Author: sandorw
 */

static move playout_policy::selectRandomMove(board_state& b) {

	//TODO:
	//What is builtin_expect?

	//TODO:
	//Should I allow filling your own eye with a low probability?

	int numEmptyLocations = b.empty_locations.size();
	int randomLoc = getRandomInt(numEmptyLocations-1);
	board_position stone_color = getStoneType(b.blacks_turn);
	bool triedAll = false;
	int i=randomLoc;
	while (!triedAll) {
		intersection loc = b.empty_locations[i];
		move m(getStoneType(b.blacks_turn), loc);
		if (b.isValidMove(m) && (b.isEye(loc) != stone_color))
			return m;
		if (++i >= numEmptyLocations)
			i=0;
		if (i == randomLoc)
			triedAll = true;
	}

	//No valid moves found, so pass
	return move(board_position::OFF_BOARD, 0);
}
