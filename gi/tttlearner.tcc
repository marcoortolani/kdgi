//#include "tttlearner.h"

#include "concretedfa.h"
#include <iostream>

using namespace std;

bool PRINT = false;

template <class O>
void TTTLearner<O> :: close_transition(TTTDfa* t, tuple<vector<string,allocator<string>>,string,bool> pair){
	vector<symbol_> prefix = std :: get<0>(pair);
	prefix.push_back(std :: get<1>(pair));

	vector<bool>queries;
	vector<symbol_> suffix;

	//if(t->init_sifting(std :: get<0>(pair), std :: get<1>(pair), suffix, std :: get<2>(pair))){

	t->init_sifting(std :: get<0>(pair), std :: get<1>(pair), suffix, std :: get<2>(pair));
		do{
			vector <symbol_> phrase = prefix;
			phrase.insert(phrase.end(), suffix.begin(), suffix.end());

			bool query;

			//if(!t->know_phrase_for_sure(phrase, query)){
				query = this->ask_membership(phrase);
			//}
			queries.push_back(query);
		}
		while(t->sift_step(suffix, queries.back(), std :: get<2>(pair)));
	//}
	t->close_transition(std :: get<0>(pair), std :: get<1>(pair), queries.back());
}

template <class O>
void TTTLearner<O> :: close_transitions(TTTDfa* t, bool hard_sift){
	for(auto vec = t->get_transitions_to_sift(hard_sift); !vec.empty(); vec = t->get_transitions_to_sift(hard_sift)){
		for (auto pair : vec){
			close_transition(t, pair);
		}
	}
	return;
}

template <class O>
TTTDfa* TTTLearner<O> :: OPACKlearn(){
	vector<bool> queries;
	vector<symbol_> alphabet = this->alphabet;
	for(symbol_ s : alphabet){
		queries.push_back(this->ask_membership((vector<symbol_>(1, s))));
	}
	queries.push_back(this->ask_membership((vector<symbol_>())));

	TTTDfa* t = new TTTDfa(alphabet, queries);
	close_transitions(t, true);

	t->make_hypotesis();

	vector<symbol_> counterexample;
	while(!this->ask_equivalence(t, counterexample)){
		bool truth = !t->membership_query(counterexample);

		while(t->membership_query(counterexample) != truth){
			t->handle_counterexample(counterexample);

			close_transitions(t, true);

			t->make_hypotesis();
		}
	}
	return t;
}

/*template <class O>
TTTDfa* TTTLearner<O> :: TTTlearn(){
	int debug = 0;
	vector<bool> queries;
	for(symbol_ s : alphabet){
		queries.push_back(this->ask_membership((vector<symbol_>(1, s))));
	}
	queries.push_back(this->ask_membership((vector<symbol_>())));

	TTTDfa* t = new TTTDfa(alphabet, queries);
	close_transitions(t, false);

	t->make_hypotesis();

	t->print_ttt_dot("", "dfas/dot/TTT" + std :: to_string(debug++) + ".dot");

	vector<symbol_> counterexample;
	while(!this->ask_equivalence_query(t, counterexample)){
		if(PRINT)
			cout << "controesempio: " << counterexample << endl;
		bool truth = !t->membership_query(counterexample);

		while(t->membership_query(counterexample) != truth){
			t->handle_counterexample(counterexample);
			close_transitions(t, false);

			if(PRINT){
			cout << debug << endl;
			t->print_ttt_dot("", "dfas/dot/TTT" + std :: to_string(debug++) + ".dot");
			}

			vector<vector<symbol_>> prefixes;
			vector<vector<symbol_>> prefixes0;
			vector<vector<symbol_>> prefixes1;

			vector<symbol_> suffix;

			while(!t->is_deterministic() && t->try_single_finalization(prefixes, prefixes0, prefixes1, suffix)){
				vector<pair<vector<symbol_>, bool>> leaf_queries;
				for(vector<symbol_> prefix : prefixes){
					vector <symbol_> phrase = prefix;
					phrase.insert(phrase.end(), suffix.begin(), suffix.end());

					leaf_queries.push_back(std :: make_pair(prefix, this->ask_membership(phrase)));
				}
				for(vector<symbol_> prefix : prefixes0){
					leaf_queries.push_back(std :: make_pair(prefix, false));
				}
				for(vector<symbol_> prefix : prefixes1){
					leaf_queries.push_back(std :: make_pair(prefix, true));
				}
				t->split_block(leaf_queries, suffix);
				close_transitions(t, false);

				cout << "finalization " << debug << endl;
				t->print_ttt_dot("", "dfas/dot/TTT" + std :: to_string(debug++) + ".dot");
			}

			if(!t->is_deterministic()){
				cout << "hard sifting " << debug << endl;
				close_transitions(t, true);
				t->print_ttt_dot("", "dfas/dot/TTT" + std :: to_string(debug++) + ".dot");
			}

			t->make_hypotesis();
		}
	}
	return t;
}*/

template <class O>
void TTTLearner<O> :: try_finalization(TTTDfa* t){
	int debug = 0;
	vector<vector<symbol_>> prefixes;
	vector<vector<symbol_>> prefixes0;
	vector<vector<symbol_>> prefixes1;

	vector<symbol_> suffix;

	while(!t->is_deterministic() && t->try_single_finalization(prefixes, prefixes0, prefixes1, suffix)){
		vector<pair<vector<symbol_>, bool>> leaf_queries;
		for(vector<symbol_> prefix : prefixes){
			vector <symbol_> phrase = prefix;
			phrase.insert(phrase.end(), suffix.begin(), suffix.end());

			leaf_queries.push_back(std :: make_pair(prefix, this->ask_membership(phrase)));
		}
		for(vector<symbol_> prefix : prefixes0){
			leaf_queries.push_back(std :: make_pair(prefix, false));
		}
		for(vector<symbol_> prefix : prefixes1){
			leaf_queries.push_back(std :: make_pair(prefix, true));
		}
		t->split_block(leaf_queries, suffix);
		close_transitions(t, false);

		if(PRINT){
			cout << "counterexample and finalization " << debug << endl;
			t->print_ttt_dot("", "dfas/dot/TTTfinalization" + std :: to_string(debug++) + ".dot");
		}
	}
}

template <class O>
TTTDfa* TTTLearner<O> :: TTTlearn_test(){
	int debug = 0;
	vector<bool> queries;
	vector<symbol_> alphabet = this->alphabet;
	for(symbol_ s : alphabet){
		queries.push_back(this->ask_membership((vector<symbol_>(1, s))));
	}
	queries.push_back(this->ask_membership((vector<symbol_>())));

	TTTDfa* t = new TTTDfa(alphabet, queries);
	close_transitions(t, false);

	t->make_hypotesis();

	if(PRINT)
		t->print_ttt_dot("", "dfas/dot/TTT" + std :: to_string(debug++) + ".dot");

	vector<symbol_> counterexample;
	while(!this->ask_equivalence(t, counterexample)){
		if(PRINT)
			cout << "controesempio: " << counterexample << endl;

		bool truth = !t->membership_query(counterexample);
		while(t->membership_query(counterexample) != truth){
			vector<symbol_> prefix;
			symbol_ transition;
			while(t->handle_counterexample_test(counterexample, truth, prefix, transition)){
				auto tuple = std :: make_tuple(prefix, transition, true);
				close_transitions(t, false);

				try_finalization(t);

				if(PRINT){
					cout << "counterexample and finalization " << debug << endl;
					t->print_ttt_dot("", "dfas/dot/TTT" + std :: to_string(debug++) + ".dot");
				}
			}

			/*if(!t->is_deterministic()){
				close_transitions(t, true);
				if(PRINT){
					cout << "hard sifting " << debug << endl;
					t->print_ttt_dot("", "dfas/dot/TTT" + std :: to_string(debug++) + ".dot");
				}
			}*/

			t->make_hypotesis();
			if(PRINT)
				t->print_dfa_dot("", "dfas/dot/Dfa" + std :: to_string(debug++) + ".dot");
		}
	}
	return t;
}

template <class O>
void TTTLearner<O> :: set_opack(bool opack){
	observation_pack_ = opack;
}

template <class O>
TTTLearner<O> :: TTTLearner(O* o, vector<string> a, bool opack) : Learner<list<DfaState>*, O> :: Learner(o, a){
	observation_pack_ = opack;
};

template <class O>
TTTDfa* TTTLearner<O> :: learn(){
	if(observation_pack_){
		return OPACKlearn();
	}
	else{
		return TTTlearn_test();
		//return TTTlearn();
	}
}