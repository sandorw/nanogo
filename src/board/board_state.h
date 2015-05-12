/*
 * board_state.h
 *
 *  Created on: Apr 21, 2015
 *      Author: sandorw
 */

#ifndef BOARD_BOARD_STATE_H_
#define BOARD_BOARD_STATE_H_

#include <params.h>
#include <vector>

typedef int intersection;

//The order of this enum is important!
//It matches the #defined WHITE/BLACK values
enum board_position {
	WHITE_STONE,
	BLACK_STONE,
	OFF_BOARD,
	EMPTY
};

board_position getOpposingColor(board_position b);

/*
 * Group class
 * Represents a collection of liberties for a group of stones
 * Does not track liberties past MAX_LIBS.
 * Not guaranteed to contain all liberties unless all_libs is true
 */
class group {
public:
	intersection liberties[MAX_LIBS];
	int n_liberties;
	bool all_libs;

	group();
	group(group& g);

	bool addLiberty(intersection x);
	bool removeLiberty(intersection x);
	void mergeGroup(group * mergeGroup);

private:

	//TODO:
	//Consider having stones in a group point to each other for faster iteration through a group

};

//Internal board size with additional padding
#define int_board_size (board_size+2)

#define num_coord (int_board_size*int_board_size)

intersection neighbor_offsets[4] = {-int_board_size, -1, 1, int_board_size};

/*
 * Move class
 * Represents a move on the board
 */
class move {
public:
	move(board_position t, intersection l);
	~move() {};

	bool isNullMove();
	bool isPass();
	board_position getStoneType();
	intersection getLocation();

private:
	intersection location;
	board_position type;
};

class board_state {
public:
	board_position coord[num_coord];
	float komi;
	intersection ko;
	move last_play, last_play2;
	int captures[2];
	bool blacks_turn;
	int group_lookup[num_coord];
	std::vector<group *> groups;

	board_state();
	board_state(board_state& b);
	~board_state();

	//returns true if the specified move is valid
	bool isValidMove(move m);

	//advances the board by applying the specified move
	//assumes that the move is valid
	void makeMove(move m);

	//returns true if the game is over
	// i.e. both players have passed
	bool isGameOver();

	//sets the komi for the game
	void setKomi(float komiVal);

	//returns true if the specified point is an eye
	bool isEye(intersection loc);

private:
	int addGroup();
	void removeGroup(int index);
	void updateGroupIndex(int oldIndex, int newIndex);
	bool isCapMove(move m);
	void refillLiberties(int groupIndex);

	//TODO: Consider having whose_turn track the allied stone and opposing stone enums pre-calculated
	//  to avoid constantly calculating them

	//TODO:
	//position hashes for pattern application
	//Zobrist hash for the entire position
	//Pachi tracks capturable groups, effectively also tracking which groups are alive
	//Pachi tracks empty intersections for fast random move generation
};

#endif /* BOARD_BOARD_STATE_H_ */
