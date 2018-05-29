/*
 * dfa.cc
 */


#include "dfa.h"

Dfa::Dfa(){
	num_states_   = 0;
	start_state_  = 0;
	alphabet_	= vector<string>();
}

Dfa::Dfa(const int n_state, const vector<string> alf, const int s_state){
	num_states_   = n_state;
	start_state_  = s_state;
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
}

Dfa::Dfa(const int n_state, const vector<string> alf)
:Dfa(n_state, alf, 0){}

Dfa::~Dfa()
{
	alphabet_.clear();
}

void Dfa::set_num_state(int n){
	num_states_ = n;
}

void Dfa::set_alphabet(const vector<string> alf)
{
	alphabet_.clear();
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
}

int Dfa::get_dim_alphabet() const{
	return alphabet_.size();
}

const vector<string> Dfa::get_alphabet() const{
	return alphabet_;
}

int Dfa::get_num_states() const{
	return num_states_;
}

int Dfa::get_start_state() const{
	return start_state_;
}
