/*
 * dfa.tcc
 */

template <class I>
Dfa<I>::Dfa(){
	//TO-DO: implementare questo controllo per ogni costruttore di Dfa (spostare quelli non necessari in ConcreteDfa)
	//Nota bene: questo controllo permette di usare solo i container sequenziali, in quanto quelli associativi richiedono
	//un ordinamento forte (non deve essere possibile l'uguaglianza su elementi diversi(?)) che non abbiamo definito per DfaState
	
	DfaState test(true, vector<symbol_>());
	static_cast<typename basetype::value_type>(test);
	
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





/* Methods related to the "dfa common interface" */

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
bool Dfa<I>::is_identical(Dfa* other_dfa, vector<symbol_>& phrase){
	vector<symbol_> sorted_alphabet = sort_alphabet();
	
	auto state_it = this->begin();
	auto other_state_it = other_dfa->begin();
	  
	while(state_it != this->end() && other_state_it != other_dfa->end()){
		DfaState state = *state_it;
		DfaState other_state = *other_state_it;
			  
		if(state.get_depth_phrase() != other_state.get_depth_phrase() || state.is_accepting() != other_state.is_accepting()){
			if(state.get_depth_phrase() == other_state.get_depth_phrase()){
				phrase = state.get_depth_phrase();
			}
			else{
				phrase = lexicographical_compare(state.get_depth_phrase().begin(), state.get_depth_phrase().end(), other_state.get_depth_phrase().begin(), other_state.get_depth_phrase().end()) ? state.get_depth_phrase() : other_state.get_depth_phrase();
			}
			return false;
		}
			  
		for(symbol_ sym : sorted_alphabet){
			DfaState* next_state = state.next(sym);
			DfaState* other_next_state = other_state.next(sym);
			
			if(next_state->get_depth_phrase() != other_next_state->get_depth_phrase() || next_state->is_accepting() != other_next_state->is_accepting()){
				phrase = state.get_depth_phrase();
				phrase.push_back(sym);
				return false;
			}
		}
		++state_it;
		++other_state_it;
	}
	if(state_it != this->end() || other_state_it != other_dfa->end()){
		if(state_it != this->end()){
			phrase = state_it->get_depth_phrase();
		}
		else{
			phrase = other_state_it->get_depth_phrase();
		}
		return false;
	}
	  
	return true;
};

template <class I>
vector<symbol_> Dfa<I>::get_next_phrase(vector<symbol_> phrase){
	vector<symbol_> sorted_alphabet = sort_alphabet();
	
	if(phrase.empty()){
		cerr << "Error in Dfa :: next_phrase: argument has length 0" << endl;
		throw 0;
	}
	
	if(phrase.back() == sorted_alphabet.back()){
		while(phrase.back() == sorted_alphabet.back()){
			phrase.pop_back();
		}
		
		if(phrase.empty()){
			cerr << "Error in Dfa :: next_phrase: phrase is now empty" << endl;
			throw 0;
		}
	}
	
	symbol_ last_sym = phrase.back();
			
	bool found_symbol = false;
	for(auto next = sorted_alphabet.begin(); next != sorted_alphabet.end() && !found_symbol; ++next){
		if(*next == last_sym){
			found_symbol = true;
			++next;
			phrase.pop_back();
			phrase.push_back(*next);
		}
	}
	
	return phrase;
}

template <class I>
template <class SymIter>
DfaState* Dfa<I>::operator[](SymIter phrase){
	return (*begin())[phrase];
}