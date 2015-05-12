/*
 * tree.h
 *
 *  Created on: May 7, 2015
 *      Author: sandorw
 */

#ifndef TREE_TREE_H_
#define TREE_TREE_H_

#include <list>

class treeNode {
public:
	move moveMade;

	//wins below are defined as the number of black wins
	float wins, sims, virt_wins, virt_sims;
	int depth;
	std::vector<treeNode *> children;
	treeNode *parent;

	//TODO:
	//Consider allocating all treeNodes in a controlled buffer
	//To do so, children will have to be a constant size
	//This is why pachi uses a pointer to the first child, then pointers to siblings

	treeNode(move newMove, int newDepth, treeNode *parentNode);
	~treeNode();

	//expands this node to include follow-up moves
	void expand(board_state& b);

	//recursively decreases the depth of the node and children by 1
	void decrementDepths();

	//returns the value of the node according to its MC playouts
	float getNodeValue(bool blacks_turn);

	//performs a MC playout from this node's position
	void performMCSimulation(board_state& b);


private:
	void addChild(move newMove, int newDepth);
};

class tree {
public:
	board_state *position;
	treeNode *root;
	std::list<move> move_history;

	tree();
	~tree();

	//Walks the tree and performs a MC playout simulation
	void performMCSimulation();

	//Applies the specified move to the board_state and tree
	void makeMove(move m);


private:


};

#endif /* TREE_TREE_H_ */
