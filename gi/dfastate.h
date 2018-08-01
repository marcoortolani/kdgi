/*
 * dfastate.h
 */

#ifndef DFASTATE_H_
#define DFASTATE_H_

#include <iostream>

#include <string>
#include <vector>
#include <map>
#include <set>

#define symbol_ string

using namespace std;

class DfaState {
private:
	bool accepting_state;
	vector<symbol_> depth_phrase;
	map<symbol_, DfaState*> transictions;
	int index_;
	vector<pair<DfaState*, symbol_> > incoming_transictions_;

public:

	DfaState(bool accepting, vector<symbol_> dep_ph, int ind);

	DfaState(bool accepting, vector<symbol_> dep_ph, map<symbol_, DfaState*> tr, int ind);

	DfaState* next(symbol_ sym);

	bool is_accepting();

	vector <symbol_> get_depth_phrase();

	void set_transiction(symbol_ sym, DfaState* arrive_state);

	void set_incoming_transictions(pair<DfaState*, symbol_> in_trans);

	vector<pair<DfaState*, symbol_> > get_incoming_transictions() const;

	vector<DfaState*> get_incoming_states() const;

	vector<DfaState*> get_outcoming_states() const;

	void print();

	int get_index() const;

	template <class Container>
	DfaState* operator[](Container phrase){
		if(phrase.empty()){
			return this;
		}
		else if(phrase.size() == 1){
			symbol_ s = static_cast<symbol_>(phrase.front());
			return next(s);
		}
		else{
			Container new_phrase;
			symbol_ s = static_cast<symbol_>(phrase.front());
			auto it = phrase.begin();
			++it;
			new_phrase.insert(new_phrase.end(), it, phrase.end());
			return (*next(s))[new_phrase];
		}
	}
};

#endif /* DFASTATE_H_ */
