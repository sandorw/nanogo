/*
 * playout_policy.cpp
 *
 *  Created on: May 11, 2015
 *      Author: sandorw
 */

static move playout_policy::selectRandomMove(board_state& b) {
	//TODO:
	//2 options here:
	//	generate a collection of all possible valid moves, then select one
	//	more memory intensive
	//		OR
	//	randomly select moves as I go, involving no extra storage, but many calls to the random number generator
	//
	//For reference, Pachi keeps track of all empty spots and iterates through them starting from a random position
	//  these are tracked at the board_state level

	//loop through the board, selecting all empty spots
	//  if an empty spot is a valid move and is not self-atari* or an eye, add it to consideration
	//		*may be allowed in some situations?
}
