

SillyOracle :: SillyOracle(ConcreteDfa* d1){
	silly_dfa = d1;
};

bool SillyOracle :: membership_query(vector <string> str){
	bool query_result = silly_dfa->membership_query(str);
	//Oracle<I> :: membership_cost(str, query_result);
    return query_result;
};

template<class Dfa>
bool SillyOracle :: equivalence_query(Dfa* dfa_hp , vector <symbol_>& witness_results){
	//Oracle<I> :: equivalence_cost();
    return silly_dfa->Dfa::equivalence_query(dfa_hp , witness_results);
};
