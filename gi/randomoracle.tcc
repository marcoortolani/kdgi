RandomOracle :: RandomOracle(int max_length, vector<symbol_> alphabet){
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0,3);

	//Scorro il numero di simboli che le frasi dovranno avere (da 0 al massimo indicato).
	for(int i = 0; i <= max_length; ++i){
		//Scorro tutti gli interi da 0 ad dim_alfabeto ^ lunghezza_frase_corrente
		//e converto il numero in base dim_alfabeto per poi sostituire ogni cifra al simbolo corrispondente.
		for(int j = 0; j < pow(alphabet.size(), i); ++j){
				int val = j;
				vector<symbol_> word;
				for(int k = 0; k < i; k++){
					word.push_back(alphabet[val % alphabet.size()]);
					val /= alphabet.size();
				}

				//Genero a caso il valore da assegnare alla parola corrente.
				bool coin_toss = false;
				if(distribution(generator) == 0)
					coin_toss = true;
					
				samples_[word] = coin_toss;
		}
	}
};


bool RandomOracle :: membership_query(vector <symbol_> phrase){
	bool query_result;

	//Se la frase è tra i sample creati allora restituisco il valore memorizzato
	auto it = samples_.find(phrase);
	if(it != samples_.end()){
		query_result = it->second;
	}
	//altrimenti restituisco falso (arbitrariamente).
	else{
		query_result = false;
	}

    return query_result;
};

template <class Dfa>
bool RandomOracle :: equivalence_query(Dfa* dfa_hp , vector <symbol_>& witness_results){
	vector<pair<vector<symbol_>, bool>> counterexamples;

	//La correttezza del dfa viene verificata (non proprio) se questo risponde correttamente ad ogni sample del RandomOracle.
	//Non viene fatto alcun controllo per le frasi che non appartengono all'insieme dei sample (tutte quelle maggiori di max_length).
	for(auto pair : samples_){
		if(dfa_hp->membership_query(pair.first) != pair.second){
			counterexamples.push_back(pair);
		}
	}

	//Dopo aver raccolto tutti i possibili controesempi se ne sceglie uno a caso da restituire.
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
