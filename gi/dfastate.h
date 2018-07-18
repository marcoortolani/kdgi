/*
 * dfastate.h
 */

#ifndef DFASTATE_H_
#define DFASTATE_H_

#include <iostream>

#include <string>
#include <vector>
#include <map>

#define symbol_ string

using namespace std;

class DfaState {
private:
	bool accepting_state;
	vector<symbol_> depth_phrase;
	map<symbol_, DfaState*> transictions;

public:

	DfaState(bool accepting, vector<symbol_> dep_ph);

	DfaState(bool accepting, vector<symbol_> dep_ph, map<symbol_, DfaState*> tr);

	DfaState* next(symbol_ sym);

	bool is_accepting();

	vector <symbol_> get_depth_phrase();

	void set_transiction(symbol_ sym, DfaState* arrive_state);

	void print();

	template <class Container>
	DfaState* operator[](Container phrase){
		if(phrase.empty()){
			return this;
		}
		else if(phrase.size() == 1){
			symbol_ s = phrase.front();
			return next(s);
		}
		else{
			Container new_phrase;
			symbol_ s = phrase.front();
			auto it = phrase.begin();
			++it;
			new_phrase.insert(new_phrase.end(), it, phrase.end());
			return (*next(s))[new_phrase];
		}
	}
};

#endif /* DFASTATE_H_ */
