/*
 * playout_policy.h
 *
 *  Created on: May 11, 2015
 *      Author: sandorw
 */

#ifndef PLAYOUT_PLAYOUT_POLICY_H_
#define PLAYOUT_PLAYOUT_POLICY_H_

//TODO: Does C++ do static classes?
//  Maybe light playouts don't require a playout_policy object, but heavy ones do?

static class playout_policy {
public:

	//chooses a random valid move from the provided board_state
	static move selectRandomMove(board_state& b);

private:

};

#endif /* PLAYOUT_PLAYOUT_POLICY_H_ */
