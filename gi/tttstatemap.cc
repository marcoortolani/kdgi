/*
 * tttstatemap.cc
 *
 *  Created on: 10 set 2018
 *      Author: marco
 */

#include "tttstatemap.h"

TTTStateMap :: TTTStateMap() : DfaState(false, vector<symbol_>(), 0){};

void TTTStateMap :: set_non_tree_transition(vector <symbol_> state_phrase, symbol_ symbol_transition, DiscrimNode* non_tree_arrive_node){
	pair<vector<symbol_>, symbol_> tr = make_pair(state_phrase, symbol_transition);
	transitions_map_[tr] = non_tree_arrive_node;
};

void TTTStateMap :: erase_non_tree_transition(vector <symbol_> state_phrase, symbol_ symbol_transition){
	transitions_map_.erase(std::make_pair(state_phrase, symbol_transition));
}

DiscrimNode* TTTStateMap :: get_non_tree_arrive_node(vector <symbol_> state_phrase, symbol_ symbol_transition) const{
	pair<vector<symbol_>, symbol_> tr = make_pair(state_phrase, symbol_transition);
	auto it = transitions_map_.find(tr);
	if(it == transitions_map_.end()){
		cerr << "Error in TTTStateMap :: get_non_tree_arrive_node : transition not found" << endl;
		throw 0;
	}

	return it->second;
};
