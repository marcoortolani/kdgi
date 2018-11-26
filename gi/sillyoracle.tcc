SillyOracle :: SillyOracle(vector<vector<symbol_>> ds, int percent){
	dataset_ = ds;
	percentage_ = percent;
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
	int count = 0;
	int err = 0;
	vector<symbol_> possible_res;
	
	for(auto el : dataset_){
	count++;
		if(! dfa_hp->membership_query(el)){
			err++;
			if(percentage_ < err * 100 / count){
				witness_results = el;
				return false;
			}
		}
	}
	
	return true;
};
