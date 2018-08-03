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
	//******** DATA MEMBERS: ********
	bool accepting_state;
	vector<symbol_> depth_phrase;
	map<symbol_, DfaState*> transictions;
	vector<pair<DfaState*, symbol_> > incoming_transictions_;
	
	int index_;

public:

	//******** CONSTRUCTORS: ********
	/**
	 * @param accepting		True if the state must be accepting, false otherwise
	 * @param dep_ph		Depth phrase, ie the first sequence in lexicographical order to reach this state
	 * @param int			A counter increasing for each new state we encounter exploring the dfa depth first
	 */
	DfaState(bool accepting, vector<symbol_> dep_ph, int ind);

	/**
	 * This constructor is now unused, and probably always will be, due to the necessity to have the references to
	 * other DfaStates of the Dfa.
	 * @param accepting		True if the state must be accepting, false otherwise
	 * @param dep_ph		Depth phrase, ie the first sequence in lexicographical order to reach this state
	 * @param tr			The map of transictions to other DfaStates for each symbol_
	 * @param int			A counter increasing for each new state we encounter exploring the dfa depth first 
	 */
	DfaState(bool accepting, vector<symbol_> dep_ph, map<symbol_, DfaState*> tr, int ind);

	//******** METHODS: ********
	/**
	 * Returns a pointer to the DfaState reached starting from this and 
	 * following the symbol_ sym
	 * @param sym	The symbol_ of the transiction
	 * @return		A pointer to the arriving DfaState of the transiction
	 */
	DfaState* next(symbol_ sym);

	/**
	 * Returns whether or not this is an accepting state
	 */
	bool is_accepting();

	/**
	 * Returns the depth_phrase ie the first sequence in lexicographical order to reach this state
	 */
	vector <symbol_> get_depth_phrase();

	/**
	 * Set a particoular transiction starting from this state
	 * @param sym			The symbol_ of the transiction
	 * @param arrive_state	A pointer to the reached DfaState
	 */
	void set_transiction(symbol_ sym, DfaState* arrive_state);

	/**
	 * Set a particoular transiction arriving to this state
	 * @param in_trans		The (pair starting_state,symbol_) of the transiction
	 */
	void set_incoming_transiction(pair<DfaState*, symbol_> in_trans);
	
	/**
	 * Get all the transictions having this state as arriving state
	 */
	vector<pair<DfaState*, symbol_> > get_incoming_transictions();

	/**
	 * Get all the incoming transictions that are removable, ie if ONE
	 * of them is shifted to another arriving state the Dfa is still a
	 * Dfa (in the sense that every state has 2 outcoming transictions
	 * and every state is reachable).
	 */
	vector<pair<DfaState*, symbol_> > get_removable_incoming_transictions();

	/**
	 * Returns the different DfaStates that have at least one transiction
	 * to this DfaState
	 */
	vector<DfaState*> get_incoming_states() const;

	/**
	 * Returns the different DfaStates that have at least one transiction
	 * coming from this DfaState
	 */
	vector<DfaState*> get_outcoming_states() const;

	/**
	 * Prints the data members of this DfaState
	 */
	void print();

	/**
	 * Returns the index of this DfaState, ie the number of states 
	 * visited before this in a depth first search of the Dfa
	 */
	int get_index() const;

	/**
	 * Returns a pointer to the DfaState reached starting from this and
	 * following a sequence of symbol_.
	 * @param phrase	The sequence of symbol_, can be (or should be) any kind of sequential container of symbol_
	 */
	template <class Container>
	DfaState* operator[](Container phrase){
		
		DfaState* res = this;
		
		while(!phrase.empty()){
			res = res->next(static_cast<symbol_>(phrase.front()));
			Container new_phrase;
			auto it = phrase.begin();
			++it;
			new_phrase.insert(new_phrase.end(), it, phrase.end());
			phrase = new_phrase;
		}
		
		return res;
	}
};

#endif /* DFASTATE_H_ */
