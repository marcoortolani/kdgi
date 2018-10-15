/*
 * learner.tcc
 */

template <class I, class O>
tuple<int, int, int, int> Learner<I,O> ::get_costs(){
	return std :: make_tuple(equivalence_cost, membership_cost, symbol_cost, repetition_cost);
}

template <class I, class O>
void Learner<I,O> :: reset_costs(){
	equivalence_cost = membership_cost = symbol_cost = repetition_cost = 0;
}

template <class I, class O>
bool Learner<I,O> :: ask_membership(vector<symbol_> phrase){
	auto query_it = asked_queries.find(phrase);
	if(query_it != asked_queries.end()){
		repetition_cost += 1;
		return query_it->second;
	}

	membership_cost += 1;
	symbol_cost += phrase.size();

	bool truth = oracle->membership_query(phrase);
	asked_queries[phrase] = truth;

	return truth;
}

template <class I, class O>
bool Learner<I,O> :: ask_equivalence(Dfa<I>* hypotesis, vector<symbol_>&counterexample){
	equivalence_cost += 1;
	return oracle->equivalence_query(hypotesis, counterexample);
}

/*template <class I, class O>
Learner<I,O>::Learner(){
    oracle = NULL;
    alphabet = vector <string>();
}*/

template <class I, class O>
Learner<I,O> :: Learner(O* o, vector <symbol_> ab){
	reset_costs();
    set_oracle(o);
    set_alphabet(ab);
}

template <class I, class O>
void Learner<I, O> :: set_oracle(O* o){
    oracle = o;
}

template <class I,class O>
void Learner<I,O> :: set_alphabet(vector<symbol_> ab){
    alphabet = ab;
}
