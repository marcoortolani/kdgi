


RandomOracle :: RandomOracle(int max_length, vector<symbol_> alphabet){
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0,1);

	for(int i = 0; i <= max_length; ++i){
		for(int j = 0; j < pow(alphabet.size(), i); ++j){
				int val = j;
				vector<symbol_> word;
				for(int k = 0; k < i; k++){
					word.push_back(alphabet[val % alphabet.size()]);
					val /= alphabet.size();
				}

				int coin_toss = distribution(generator);
				samples_[word] = static_cast<bool>(coin_toss);
		}
	}
};


bool RandomOracle :: membership_query(vector <string> str){
	bool query_result;

	auto it = samples_.find(str);
	if(it != samples_.end()){
		query_result = it->second;
	}
	else{
		/*cout << "generato nuovo sample on the fly" << endl;
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(0,1);
		query_result = static_cast<bool>(distribution(generator));
		samples_[str] = query_result;*/
		query_result = false;
	}

	//Oracle<I> :: membership_cost(str, query_result);
    return query_result;
};

template <class Dfa>
bool RandomOracle :: equivalence_query(Dfa* dfa_hp , vector <symbol_>& witness_results){
	//Oracle<I> :: equivalence_cost();

	vector<pair<vector<symbol_>, bool>> counterexamples;

	for(auto pair : samples_){
		if(dfa_hp->membership_query(pair.first) != pair.second){
			counterexamples.push_back(pair);
		}
	}

	if(!counterexamples.empty()){
		std::default_random_engine generator;
		std::uniform_int_distribution<int> distribution(0,counterexamples.size());
		vector<symbol_> random_counterexample = counterexamples[distribution(generator)].first;
		witness_results = random_counterexample;
		return false;
	}

    return true;
};


void RandomOracle :: print(){
	for(auto sample : samples_)
		cout << sample.first << " : " << sample.second << endl;
}
