SillyOracle :: SillyOracle(vector<vector<symbol_>> ds){
	dataset_ = ds;
};

bool SillyOracle :: membership_query(vector <symbol_> str){
	for(auto el : dataset_){
		if(el == str)
			return true;
	}

    return false;
};

template<class Dfa>
bool SillyOracle :: equivalence_query(Dfa* dfa_hp , vector <symbol_>& witness_results){
	for(auto el : dataset_){
		if(! dfa_hp->membership_query(el)){
			witness_results = el;
			return false;
		}
	}
	
	return true;
};
