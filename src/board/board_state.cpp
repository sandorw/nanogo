/*
 * board_state.cpp
 *
 *  Created on: Apr 21, 2015
 *      Author: sandorw
 */

#include <board_state.h>
#include <set>
#include <stack>

board_position getStoneType(bool blacks_turn) {
	return (blacks_turn ? board_position::BLACK_STONE : board_position::WHITE_STONE);
}

board_position getOpposingColor(board_position b) {
	return (b == board_position::WHITE_STONE ? board_position::BLACK_STONE : board_position::WHITE_STONE);
}

group::group() {
	n_liberties = 0;
	all_libs = true;
}

group::group(group& g) {
	n_liberties = g.n_liberties;
	all_libs = g.all_libs;
	for (int i=0; i < n_liberties; ++i)
		liberties[i] = g.liberties[i];
}

bool group::addLiberty(intersection x) {
	if (n_liberties == MAX_LIBS) {
		all_libs = false;
		return false;
	}
	for (int i=0; i < n_liberties; ++i)
		if (liberties[i] == x)
			return n_liberties < REFILL_LIBS;
	liberties[n_liberties++] = x;
	return n_liberties < REFILL_LIBS;
}

bool group::removeLiberty(intersection x) {
	for (int i=0; i < n_liberties; ++i)
		if (liberties[i] == x) {
			liberties[i] = liberties[--n_liberties];
			break;
		}
	return n_liberties < REFILL_LIBS;
}

void group::mergeGroup(group * mergeGroup) {
	for (int i=0; i < mergeGroup->n_liberties; ++i)
		addLiberty(mergeGroup->liberties[i]);
}

move::move(board_position t, intersection l) {
	type = t;
	location = l;
}

bool move::isNullMove() {
	return (type == board_position::EMPTY);
}

bool move::isPass() {
	return (type == board_position::OFF_BOARD);
}

board_position move::getStoneType() {
	return type;
}

intersection move::getLocation() {
	return location;
}

board_state::board_state() {
	for (int i=0; i < num_coord; ++i)
		coord[i] = board_position::EMPTY;
	for (int i=0; i < int_board_size; ++i)
		coord[i] = board_position::OFF_BOARD;
	for (int i=num_coord-int_board_size; i < num_coord; ++i)
		coord[i] = board_position::OFF_BOARD;
	for (int i=0; i < num_coord; i += int_board_size)
		coord[i] = board_position::OFF_BOARD;
	for (int i=int_board_size-1; i < num_coord; i += int_board_size)
		coord[i] = board_position::OFF_BOARD;
	for (int i=0; i < num_coord; ++i)
		if (coord[i] == board_position::EMPTY)
			addEmptyLocation(i);
	komi = initial_komi;
	ko = -1;
	last_play = last_play2 = move(board_position::EMPTY, 0);
	captures[WHITE] = captures[BLACK] = 0;
	blacks_turn = true;
	for (int i=0; i < num_coord; ++i)
		group_lookup[i] = -1;
}

board_state::board_state(board_state& b) {
	for (int i=0; i < num_coord; ++i) {
		coord[i] = b.coord[i];
		group_lookup[i] = b.group_lookup[i];
	}
	komi = b.komi;
	ko = b.ko;
	last_play = b.last_play;
	last_play2 = b.last_play2;
	captures[0] = b.captures[0];
	captures[1] = b.captures[1];
	blacks_turn = b.blacks_turn;
	for (int i=0; i < b.groups.size(); ++i) {
		groups.push_back(new group(b.groups[i]));
	}
	for (intersection loc : b.empty_locations)
		addEmptyLocation(loc);
}

board_state::~board_state() {
	for (int i=0; i < groups.size(); ++i)
		delete groups[i];
	groups.clear();
}

void board_state::setKomi(float komiVal) {
	komi = komiVal;
}

bool board_state::isValidMove(move m) {
	intersection loc = m.getLocation();
	if (((coord[loc] != board_position::EMPTY) && (!m.isPass())) ||
			(loc == ko) ||
			(m.isNullMove()))
		return false;
	bool has_liberties = false;
	for (int i=0; i < 4; ++i)
		if (coord[loc + neighbor_offsets[i]] == board_position::EMPTY) {
			has_liberties = true;
			break;
		}
	if (!has_liberties) {
		//Possible suicide move if this is the last liberty of all attached groups
		bool is_suicide = true;
		for (int i=0; i < 4; ++i) {
			if (coord[loc + neighbor_offsets[i]] == m.getStoneType()) {
				group* neighbor_group = groups[group_lookup[loc + neighbor_offsets[i]]];
				if (neighbor_group->n_liberties > 1) {
					is_suicide = false;
					break;
				}
			}
		}
		if (is_suicide && !isCapMove(m)) {
			//Valid move only if it captures an opposing group
			return false;
		}
	}
	return true;
}

bool board_state::isCapMove(move m) {
	intersection loc = m.getLocation();
	board_position opposingColor = getOpposingColor(m.getStoneType());
	for (int i=0; i < 4; ++i)
		if ((coord[loc + neighbor_offsets[i] == opposingColor]) &&
				(groups[group_lookup[loc + neighbor_offsets[i]]]->n_liberties == 1))
			return true;
	return false;
}

void board_state::makeMove(move m) {
	if (m.isNullMove())
		return;
	ko = -1;
	last_play2 = last_play;
	last_play = m;
	board_position stoneColor = m.getStoneType();
	if (m.isPass() || ((stoneColor != board_position::WHITE_STONE) && (stoneColor != board_position::BLACK_STONE)))
		return;
	intersection loc = m.getLocation();
	board_position opposingColor = getOpposingColor(stoneColor);
	coord[loc] = stoneColor;
	removeEmptyLocation(loc);
	int groupIndex = group_lookup[loc] = addGroup();
	bool check_for_ko = false;
	for (int i=0; i < 4; ++i) {
		intersection neighborIndex = loc + neighbor_offsets[i];
		board_position neighbor = coord[neighborIndex];
		switch (neighbor) {

		case board_position::EMPTY:
			groups[groupIndex]->addLiberty(neighborIndex);
			break;

		case stoneColor:
			groups[neighborIndex]->removeLiberty(loc);
			if (groupIndex < neighborIndex) {
				groups[groupIndex]->mergeGroup(groups[neighborIndex]);
				updateGroupIndex(neighborIndex,groupIndex);
				removeGroup(neighborIndex);
			} else {
				groups[neighborIndex]->mergeGroup(groups[groupIndex]);
				updateGroupIndex(groupIndex,neighborIndex);
				removeGroup(groupIndex);
				groupIndex = neighborIndex;
			}
			refillLiberties(groupIndex);
			break;

		case opposingColor:
			groups[neighborIndex]->removeLiberty(loc);
			refillLiberties(neighborIndex);
			if (groups[neighborIndex]->n_liberties == 0) {
				//This group has been captured
				int num_capped = 0;
				int capturedGroup = group_lookup[neighborIndex];
				for (int j=0; j < num_coord; ++j) {
					if (groups[j] == capturedGroup) {
						coord[j] = board_position::EMPTY;
						addEmptyLocation(j);
						group_lookup[j] = -1;
						captures[stoneColor]++;
						num_capped++;
						for (int k=0; k < 4; ++k)
							if (coord[j + neighbor_offsets[k]] == stoneColor)
								groups[j + neighbor_offsets[k]]->addLiberty(j);
					}
				}
				check_for_ko = (num_capped == 1);
				removeGroup(capturedGroup);
			}
			break;
		}
	}
	if (check_for_ko) {
		//If the newly played move:
		// captured a single stone
		// is not connected to an allied group
		// has only a single liberty
		//then that liberty represents a ko position
		intersection liberty_index = -1;
		for (int i=0; i < 4 && check_for_ko; ++i) {
			if (coord[loc + neighbor_offsets[i]] == stoneColor) {
				check_for_ko = false;
				break;
			} else if (coord[loc + neighbor_offsets[i]] == board_position::EMPTY) {
				if (liberty_index == -1) {
					liberty_index = loc + neighbor_offsets[i];
				} else {
					check_for_ko = false;
					break;
				}
			}
		}
		if (check_for_ko)
			ko = liberty_index;
	}
	//Toggle whose turn it is
	blacks_turn = !blacks_turn;
}

bool board_state::isGameOver() {
	return (last_play.isPass() && last_play2.isPass());
}

int board_state::addGroup() {
	groups.push_back(new group());
	return groups.size()-1;
}

void board_state::removeGroup(int index) {
	int lastIndex = groups.size()-1;
	delete groups[index];
	groups[index] = groups[lastIndex];
	updateGroupIndex(lastIndex,index);
	groups.pop_back();
}

void board_state::updateGroupIndex(int oldIndex, int newIndex) {
	for (int i=0; i < num_coord; ++i)
		if (group_lookup[i] == oldIndex)
			group_lookup[i] = newIndex;
}

void board_state::refillLiberties(int groupIndex) {
	if (groups[groupIndex]->all_libs || groups[groupIndex]->n_liberties >= REFILL_LIBS)
		return;
	groups[groupIndex]->all_libs = true;
	for (int i=0; i < num_coord; ++i) {
		if (group_lookup[i] == groupIndex) {
			for (int j=0; j < 4; ++j)
				if (coord[i + neighbor_offsets[j]] == board_position::EMPTY)
					groups[groupIndex]->addLiberty(i + neighbor_offsets[j]);
		}
	}
}

board_position board_state::isPotentialEye(intersection loc) {
	if (coord[loc] != board_position::EMPTY)
			return board_position::EMPTY;
	board_position eye_color = board_position::EMPTY;
	for (int i=0; i < 4; ++i) {
		board_position bp = coord[loc + neighbor_offsets[i]];
		if (bp == board_position::EMPTY)
			return board_position::EMPTY;
		if (bp == board_position::OFF_BOARD || bp == eye_color)
			continue;
		if (eye_color == board_position::EMPTY)
			eye_color = bp;
		else if (bp != eye_color)
			return board_position::EMPTY;
	}
	return eye_color;
}

bool board_state::isFalseEye(intersection loc, board_position eye_color) {
	//This method counts empty spots as the color of the eye
	//It could return eyes that are not yet finished and could become false

	//Possible false positives from this method for some board states

	int opposing_count = 0;
	bool atEdge = false;
	board_position opposing_color = getOpposingColor(eye_color);
	for (int i=0; i < 4; ++i) {
		board_position bp = coord[loc + diag_offsets[i]];
		if (bp == opposing_color)
			++opposing_count;
		else if (bp == board_position::OFF_BOARD)
			atEdge = true;
	}
	if (atEdge)
		++opposing_count;
	return (opposing_count >= 2);
}

board_position board_state::isEye(intersection loc) {
	board_position bp = isPotentialEye(loc);
	if ((bp == board_position::EMPTY) || (isFalseEye(loc, bp)))
		return board_position::EMPTY;
	return bp;
}

inline void board_state::addEmptyLocation(intersection loc) {
	empty_locations.push_back(loc);
}

inline void board_state::removeEmptyLocation(intersection loc) {
	for (int i=0; i < empty_locations.size(); ++i)
		if (empty_locations[i] == loc) {
			empty_locations[i] = empty_locations.back();
			empty_locations.pop_back();
		}
}

std::pair<float,float> board_state::scoreChinese() {
	//This assumes that all dead groups have already been captured and removed from the board
	float wscore = komi;
	float bscore = 0;

	//TODO:
	//  Handicap score in stones should be added to white's score

	std::set<intersection> processedEmpties, emptiesGroup;
	std::stack<intersection> awaitingProcessing;
	for (int i=0; i < num_coord; ++i) {
		if (coord[i] == board_position::WHITE_STONE)
			wscore++;
		else if (coord[i] == board_position::BLACK_STONE)
			bscore++;
		else if ((coord[i] == board_position::EMPTY)
				&& (processedEmpties.find(i) == processedEmpties.end())) {
			//Determine if this position and any linked positions should count as territory for either player
			emptiesGroup.clear();
			awaitingProcessing.push(i);
			bool isClaimed = true;
			board_position currentColor = board_position::EMPTY;
			while (!awaitingProcessing.empty()) {
				intersection loc = awaitingProcessing.pop();
				emptiesGroup.insert(loc);
				for (int i=0; i < 4; ++i) {
					intersection neighbor = loc + neighbor_offsets[i];
					board_position neighbor_color = coord[neighbor];
					//Check below means there could be multiple copies of a location on the stack
					if ((neighbor_color == board_position::EMPTY) && (emptiesGroup.find(neighbor) == emptiesGroup.end()))
						awaitingProcessing.push(neighbor);
					else if ((neighbor_color == board_position::WHITE_STONE) || (neighbor_color == board_position::BLACK_STONE)) {
						if (currentColor == board_position::EMPTY)
							currentColor = neighbor_color;
						else if (currentColor != neighbor_color)
							isClaimed = false;
					}
				}
			}
			for (intersection loc : emptiesGroup) {
				processedEmpties.insert(loc);
				if (isClaimed) {
					if (currentColor == board_position::WHITE_STONE)
						wscore++;
					else if (currentColor == board_position::BLACK_STONE)
						bscore++;
				}
			}
		}
	}

	return std::make_pair(bscore, wscore);
}
