/*
 * dfastate.cc
 */

#include "dfastate.h"

DfaState::DfaState(bool final, vector<symbol_> dep_ph, int ind){
	accepting_state = final;
	depth_phrase = dep_ph;
	index_ = ind;
}

DfaState::DfaState(bool final, vector<symbol_> dep_ph, map<symbol_, DfaState*> tr, int ind):DfaState(final, dep_ph, ind){
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

int DfaState::get_index() const{
	return index_;
}

void DfaState::set_incoming_transictions(pair<DfaState*, symbol_> in_trans){
	incoming_transictions_.push_back(in_trans);
}

vector<pair<DfaState*, symbol_> > DfaState::get_incoming_transictions(){
	return incoming_transictions_;
}

vector<pair<DfaState*, symbol_> > DfaState::get_removable_incoming_transictions(){
	// per lo stato iniziale possiamo rimuovere una qualunque delle sue transizioni entranti
	if(this->get_depth_phrase().empty())
		return incoming_transictions_;

	vector<pair<DfaState*, symbol_> > from_self;
	vector<pair<DfaState*, symbol_> > from_others;
	for(auto coppia : incoming_transictions_)
		if(coppia.first->get_depth_phrase() == this->get_depth_phrase())
			from_self.push_back(coppia);
		else
			from_others.push_back(coppia);

	if(from_others.size() == 0)
		cerr << "Lo stato " << this->get_index()<< " non è lo stato iniziale, ma non ha transizoni entranti." <<endl;
	
	if(from_others.size() == 1)
		return from_self;

	if(from_others.size() > 1){
		from_others.insert(from_others.end(), from_self.begin(), from_self.end());
		return from_others;
	}

	
}

vector<DfaState*> DfaState::get_incoming_states() const{
	set<DfaState*> accumulatore;
	for(pair<DfaState*,symbol_> coppia : incoming_transictions_)
		accumulatore.insert(coppia.first);
	vector<DfaState*> v;
	v.insert(v.end(), accumulatore.begin(), accumulatore.end());
	return v;
}

vector<DfaState*> DfaState::get_outcoming_states() const{
	set<DfaState*> accumulatore;
	for(auto coppia : transictions)
		accumulatore.insert(coppia.second);
	vector<DfaState*> v;
	v.insert(v.end(), accumulatore.begin(), accumulatore.end());
	return v;
}