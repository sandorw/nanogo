/*
 * tree.cpp
 *
 *  Created on: May 7, 2015
 *      Author: sandorw
 */

#include <board_state.h>
#include <cmath>

treeNode::treeNode(move newMove, int newDepth, treeNode *parentNode) {
	moveMade = newMove;
	wins = sims = virt_wins = virt_sims = 0.0f;
	depth = newDepth;
	parent = parentNode;
}

treeNode::~treeNode() {
	for (int i=0; i < children.size(); ++i)
		delete children[i];
}

void treeNode::addChild(move newMove, int newDepth) {
	children.push_back(new treeNode(newMove, newDepth, this));
}

void treeNode::expand(board_state& b) {
	if (b.isGameOver() || (depth == MAX_DEPTH))
		return;
	addChild(move(board_position::OFF_BOARD, 0), depth+1);
	for (int i=0; i < num_coord; ++i)
		if (b.coord[i] == board_position::EMPTY) {
			move m = move((b.blacks_turn ? board_position::BLACK_STONE : board_position::WHITE_STONE), i);
			if (b.isValidMove(m))
				addChild(m, depth+1);
		}
}

void treeNode::decrementDepths() {
	--depth;
	for (auto child : children)
		child->decrementDepths();
}

float treeNode::getNodeValue(bool blacks_turn) {
	int effective_sims = sims + virt_sims;
	float effective_wins;
	if (blacks_turn)
		effective_wins = wins*(sims/effective_sims) + virt_wins*(virt_sims/effective_sims);
	else
		effective_wins = (sims-wins)*(sims/effective_sims) + (virt_sims-virt_wins)*(virt_sims/effective_sims);
	return effective_wins/effective_sims + EXPLORATION_CONSTANT*sqrt(log(parent->sims)/sims);
}

void treeNode::performMCSimulation(board_state& b) {
	//TODO: this

	//Until the game is finished,
	//  generate a move, then apply that move
	//move generation can be via a number of strategies!

	//When the game is finished,
	//	score the current board_state and assign a winner
	bool blackWins;

	//Update the tree with the results of the simulation
	treeNode *node = this;
	while (node != nullptr) {
		if (blackWins)
			node->wins++;
		node->sims++;
		node = node->parent;
	}
}

tree::tree() {
	position = new board_state();
	root = new treeNode(move(board_position::EMPTY, 0), 0, nullptr);
}

tree::~tree() {
	delete position;
	delete root;
	move_history.clear();
}

void tree::makeMove(move m) {
	if (m.isNullMove())
		return;

	//Prune the tree of invalid nodes and advance the root of the tree
	treeNode *newRoot = nullptr;
	for (std::iterator i=root->children.begin(); i < root->children.end(); ++i) {
		treeNode *child = *i;
		if (child->moveMade == m) {
			newRoot = child;
			root->children.erase(i);
			break;
		}
	}
	delete root;
	if (newRoot == nullptr)
		newRoot = new treeNode(m, 0, nullptr);
	else
		newRoot->decrementDepths();
	root = newRoot;

	position->makeMove(m);
	move_history.push_back(m);
}

void tree::performMCSimulation() {
	board_state b = board_state(position);

	//Walk the tree until we hit a leaf node
	treeNode *playoutNode = root;
	treeNode *bestNode;
	int numNodes;
	float bestVal, nodeVal;
	while (playoutNode->children.size() > 0) {
		bestNode = nullptr;
		bestVal = -1.0f;
		for (auto child : playoutNode->children) {
			nodeVal = child->getNodeValue(b.blacks_turn);
			if (nodeVal > bestVal) {
				bestVal = nodeVal;
				bestNode = child;
				numNodes = 1;
			} else if ((nodeVal == bestVal) && (RAND < 1.0f/(++numNodes))) {

				//TODO: float equality checking above is bad

				//Randomly choose a node among nodes with the same value
				//Note: this potentially calls the random number generator frequently, but should be a rare occurrence
				bestNode = child;
			}
		}
		playoutNode = bestNode;
		b.makeMove(playoutNode->moveMade);
	}

	//Perform the simulation from the leaf node
	playoutNode->performMCSimulation(b);

	//Expand the leaf node as appropriate
	if (playoutNode->sims >= EXPAND_NODE_THRESHOLD)
		playoutNode->expand(b);

}
