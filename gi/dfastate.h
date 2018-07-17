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
};

#endif /* DFASTATE_H_ */
