/*
 * dfastate.cc
 */

#include "dfastate.h"

DfaState::DfaState(bool final, vector<symbol_> dep_ph){
	accepting_state = final;
	depth_phrase = dep_ph;
}

DfaState::DfaState(bool final, vector<symbol_> dep_ph, map<symbol_, DfaState*> tr):DfaState(final, dep_ph){
	transictions = tr;
}

DfaState* DfaState::next(symbol_ sym){
	auto it = transictions.find(sym);

	if(it == transictions.end()){
		cerr << "Error in DfaState::next, symbol " << sym << " not found" << endl;
		throw 0;
	}

	return it->second;
}

bool DfaState::is_accepting(){
	return accepting_state;
}

vector <symbol_> DfaState::get_depth_phrase(){
	return depth_phrase;
}

void DfaState::print(){
	cout << "+-----------------------+" << endl;
	cout << " depth phrase: \"";
	for(auto sym : depth_phrase){
		cout << sym;
	}
	cout << "\"" << endl;
	cout << " is accepting: " << accepting_state << endl;
	for(auto pair : transictions){
		cout << "\t" << pair.first << "\t";
		for(auto sym : pair.second->depth_phrase){
			cout << sym;
		}
		cout << endl;
	}
	cout << "+-----------------------+" << endl;
}

void DfaState::set_transiction(symbol_ sym, DfaState* arrive_state){
	transictions[sym] = arrive_state;
}
