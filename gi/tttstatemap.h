/*
 * tttstatemap.h
 *
 *  Created on: 10 set 2018
 *      Author: marco
 */

#ifndef TTTSTATEMAP_H_
#define TTTSTATEMAP_H_

#include <map>
#include <utility>

#include "dfastate.h"
#include "discrimnode.h"

class TTTStateMap : public DfaState{
private:
	map<pair<vector<symbol_>, symbol_>, DiscrimNode*> transitions_map_;

public:
	TTTStateMap();
	void set_non_tree_transition(vector <symbol_> state_phrase, symbol_ symbol_transition, DiscrimNode* non_tree_arrive_node);
	void erase_non_tree_transition(vector <symbol_> state_phrase, symbol_ symbol_transition);
	DiscrimNode* get_non_tree_arrive_node(vector <symbol_> state_phrase, symbol_ symbol_transition) const;
};

#endif /* TTTSTATEMAP_H_ */
