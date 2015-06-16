/*
 * nanogo.cpp
 *
 *  Created on: May 8, 2015
 *      Author: sandorw
 */

#include <chrono>
#include "params.h"

nanogo::nanogo() {
	search_tree = new tree();

	//set komi?
}

nanogo::~nanogo() {
	delete search_tree;
}

void nanogo::playGame() {
	//Example prototype for how a game might be played

	//TODO:
	//Add support for player input instead of MC simulations

	board_state b = *search_tree->position;
	while (!b.isGameOver()) {
		//Take a single turn

		auto limit = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()
			+ std::chrono::seconds(TIME_PER_TURN_S));
		while (std::chrono::system_clock::now() < limit) {
			search_tree->performMCSimulation();
		}
		move m = search_tree->chooseMove();
		search_tree->makeMove(m);

	}


}


