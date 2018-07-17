/*
 * dfa.tcc
 */

template <class I>
Dfa<I>::Dfa(){
	num_states_   = 0;
	start_state_  = 0;
	alphabet_	= vector<symbol_>();
}


template <class I>
Dfa<I>::Dfa(const int n_state, const vector<symbol_> alf, const int s_state){
	num_states_   = n_state;
	start_state_  = s_state;
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
}

template <class I>
Dfa<I>::Dfa(const int n_state, const vector<symbol_> alf)
:Dfa(n_state, alf, 0){}

template <class I>
Dfa<I>::~Dfa()
{
	alphabet_.clear();
}

template <class I>
void Dfa<I>::set_num_state(int n){
	num_states_ = n;
}

template <class I>
void Dfa<I>::set_alphabet(const vector<symbol_> alf)
{
	alphabet_.clear();
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
}

template <class I>
int Dfa<I>::get_dim_alphabet() const{
	return alphabet_.size();
}

template <class I>
const vector<symbol_> Dfa<I>::get_alphabet() const{
	return alphabet_;
}

template <class I>
int Dfa<I>::get_num_states() const{
	return num_states_;
}

template <class I>
int Dfa<I>::get_start_state() const{
	return start_state_;
}





/* New code here */

template <class I>
vector<symbol_> Dfa<I>::sort_alphabet(){
	//bubblesort
	vector<symbol_> sorted_alphabet = get_alphabet();
	bool sorted = false;

	while(!sorted){
		sorted = true;
		for(int i = 0; i < sorted_alphabet.size() - 1 && sorted; ++i){
			if(!lexicographical_compare(sorted_alphabet.begin() + i, sorted_alphabet.begin() + i+1, sorted_alphabet.begin() + i+1, sorted_alphabet.begin() + i+2)){
				sorted = false;
				symbol_ temp = sorted_alphabet[i];
				sorted_alphabet[i] = sorted_alphabet[i+1];
				sorted_alphabet[i+1] = temp;
			}
		}
	}
	return sorted_alphabet;
}

template <class I>
bool Dfa<I>::is_identical(Dfa* other_dfa){
	  auto state_it = this->begin();
	  auto other_state_it = other_dfa->begin();

	  //old code
	  /*while(state_it != this->end() && other_state_it != other_dfa->end()){
		  auto transiction_it = state_it->begin();
		  auto other_transiction_it = other_state_it->begin();

		  while(transiction_it != state_it->end() && other_transiction_it != other_state_it->end()){
			  auto symbol_it = transiction_it->begin();
			  auto other_symbol_it = other_transiction_it->begin();

			  while(symbol_it != transiction_it->end() && other_symbol_it != other_transiction_it->end()){

				  if(*symbol_it != *other_symbol_it){
					  return false;
				  }
				  ++symbol_it;
				  ++other_symbol_it;
			  }
			  if(symbol_it != transiction_it->end() || other_symbol_it != other_transiction_it->end()){
				  return false;
			  }
			  ++transiction_it;
			  ++other_transiction_it;
		  }
		  if(transiction_it != state_it->end() || other_transiction_it != other_state_it->end()){
			  return false;
		 }
		  ++state_it;
		  ++other_state_it;
	  }
	  if(state_it != this->end() || other_state_it != other_dfa->end()){
		  return false;
	  }
	  return true;*/
	  
	  while(state_it != this->end() && other_state_it != other_dfa->end()){
		  DfaState state = *state_it;
		  DfaState other_state = *other_state_it;
		  
		  if(state.get_depth_phrase() != state.get_depth_phrase() || state.is_accepting() != other_state.is_accepting()){
			  return false;
		  }
		  
		  ++state_it;
		  ++other_state_it;
	  }
	  if(state_it != this->end() || other_state_it != other_dfa->end()){
		  return false;
	  }
	  
	  return true;
};