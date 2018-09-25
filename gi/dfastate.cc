/*
 * dfastate.cc
 */

#include "dfastate.h"

DfaState::DfaState(bool final, vector<symbol_> dep_ph, int ind){
	accepting_state = final;
	char_phrase = dep_ph;
	index_ = ind;
}

DfaState::DfaState(bool final, vector<symbol_> dep_ph, map<symbol_, DfaState*> tr, int ind):DfaState(final, dep_ph, ind){
	transictions = tr;
}

DfaState* DfaState::next(symbol_ sym, bool strict){
	auto it = transictions.find(sym);

	if(it == transictions.end()){
		if(strict){
			cerr << "Error in DfaState::next, symbol " << sym << " not found" << endl;
			cerr << "you may want to set the parameter 'strict' to false" << endl;
			cerr << "and manage the return value" << endl;
			throw 0;
		}
		else{
			return NULL;
		}
	}

	return it->second;
}

bool DfaState::is_accepting(){
	return accepting_state;
}

vector <symbol_> DfaState::get_charact_phrase(){
	return char_phrase;
}

void DfaState::print(){
	cout << "+-----------------------+" << endl;
	cout << " depth phrase: \"";
	for(auto sym : char_phrase){
		cout << sym;
	}
	cout << "\"" << endl;
	cout << " is accepting: " << accepting_state << endl;
	for(auto pair : transictions){
		cout << "\t" << pair.first << "\t";
		for(auto sym : pair.second->char_phrase){
			cout << sym;
		}
		cout << endl;
	}
	cout << "+-----------------------+" << endl;
}

void DfaState::set_transition(symbol_ sym, DfaState* arrive_state){
	transictions[sym] = arrive_state;
}

int DfaState::get_index() const{
	return index_;
}

void DfaState::set_incoming_transiction(pair<DfaState*, symbol_> in_trans){
	incoming_transictions_.push_back(in_trans);
}

vector<pair<DfaState*, symbol_> > DfaState::get_incoming_transictions(){
	return incoming_transictions_;
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

vector<pair<DfaState*, symbol_> > DfaState::get_removable_incoming_transictions(){
	// per lo stato iniziale possiamo rimuovere una qualunque delle sue transizioni entranti
	if(this->get_charact_phrase().empty())
		return incoming_transictions_;

	//Dividiamo le transizioni entranti in quelle da se stessi e da altri stati
	vector<pair<DfaState*, symbol_> > from_self;
	vector<pair<DfaState*, symbol_> > from_others;
	for(auto coppia : incoming_transictions_)
		if(coppia.first->get_charact_phrase() == this->get_charact_phrase())
			from_self.push_back(coppia);
		else
			from_others.push_back(coppia);

	if(from_others.size() == 0){
		cerr << "Error in DfaState::get_removable_incoming_transictions" <<endl;
		cerr << "DfaState " << this->get_index() << endl;
		cerr << "is neither reachable nor first" << endl;
	}
	
	//Se abbiamo una sola transizione entrante da altri stati e non siamo lo stato iniziale
	//quella transizione non può essere rimossa
	if(from_others.size() == 1)
		return from_self;

	//Se abbiamo almeno 2 transizioni entranti se ne può rimuovere una qualunque.
	if(from_others.size() > 1){
		from_others.insert(from_others.end(), from_self.begin(), from_self.end());
		return from_others;
	}

	
}
