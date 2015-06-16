/*
 * nanogo.h
 *
 *  Created on: May 8, 2015
 *      Author: sandorw
 */

#ifndef MAIN_NANOGO_H_
#define MAIN_NANOGO_H_

class nanogo {
public:
	tree *search_tree;

	nanogo();
	~nanogo();
	void playGame();

private:

};

#endif /* MAIN_NANOGO_H_ */

//TODO: fast random number generation (Easy)

//TODO: figure out how to set and vary settings in params.h
//  Do I want these #defined? That prohibits run time configuration
//  Figure out how best to configure things, including which modules to use

//TODO: (Medium)
//Take a look at the overall control flow
//Remaining tasks:
//  random playout strategy (simplest and fastest to implement)
//  enable the playout to detect when it is done and score the board
//		play all non-eye locations (plus avoid self atari) until the board is full

//TODO: RAVE/AMAF propagation (Medium)
//  include flags to turn it on/off
//  update sibling nodes if you made one of their moves in your playout simulation
//  IGNORE this until heuristics are implemented - not a good addition to random playouts
//  Pachi includes criticality information in the RAVE setup (thesis page 47)

//TODO: plan for seki and self-atari
//  avoiding self-atari does prevent messing up seki

//TODO: Edge cases in eye/false eye implementation
//  double dragon case?
//  consider two groups facing each other, not connected but sharing 2 liberties - eyes look false but aren't

//TODO: GTP interfacing (Very Hard)

//TODO: New move priors (Very Hard)
//Newly expanded nodes need some initial weight applied to them via virtual wins and simulations
//Pachi uses heuristics here (similar to the MC playout heuristics)

//TODO: Heavy playouts via heuristics (Very Hard)
//This will probably be the most complicated part of the code

//TODO:
//Things mentioned in Pachi that need more investigation:
//  dynamic komi (MCTS operates best near the win/lose point, so adjust komi to compensate)
//  local value (try to understand the importance of playing a point by studying the end board situation)
//  criticality (correlate owning certain intersections with winning the game)
//  superko
//  value scaling (wins count for 1 +/- a bit depending on how much you win by
