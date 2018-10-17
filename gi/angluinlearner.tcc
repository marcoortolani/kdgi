//#include "angluinlearner.h"

#include <iostream>

using namespace std;

template <class O>
void AngluinLearner<O> :: make_mem_queries(map<list<string>, bool>* mem_query_res){
	for(auto pair : *mem_query_res){
		vector<string> vec;
		vec.insert(vec.end(), pair.first.begin(), pair.first.end());
		(*mem_query_res)[pair.first] = (this->ask_membership(vec));
	}
}

template <class O>
AngluinLearner<O> :: AngluinLearner(O* o, vector <symbol_> ab) : Learner<vector<DfaState>*, O> :: Learner(o, ab){};

template <class O>
AngluinDfa* AngluinLearner<O> :: learn(){
	map <list<string>, bool>* mem_query_res = new map<list<string>, bool>;
	//TO-DO: cercare di risolvere questa "ridondanza"
	vector <symbol_> str_vec;
	list <symbol_> str_lis;
	vector<symbol_> alpha = this->alphabet;
	for(unsigned int i=0; i < alpha.size(); i++){
		str_vec = vector<string>(1, alpha[i]);
		str_lis = list<string>(1, alpha[i]);
		(*mem_query_res)[str_lis] = this->ask_membership(str_vec);
	}
	str_vec = vector <string>(0);
	str_lis = list<symbol_>(1, "");
	(*mem_query_res)[str_lis] = this->ask_membership(str_vec);
	AngluinDfa* ang_dfa = new AngluinDfa(alpha, mem_query_res);
	delete(mem_query_res);

	while(true){
		bool is_consistent = false;
		bool is_closed = false;

		while(!is_closed || !is_consistent){
			if(!is_consistent){
				mem_query_res = new map<list<string>, bool>;
				is_consistent = ang_dfa->get_inconsistent_phrases(mem_query_res);
				if(!is_consistent){
					make_mem_queries(mem_query_res);
					ang_dfa->extend_column(mem_query_res);
					is_closed = false;
				}
				free(mem_query_res);
			}

			if(!is_closed){
				mem_query_res = new map<list<string>, bool>;
				is_closed = ang_dfa->get_open_phrases(mem_query_res);
				if(!is_closed){
					make_mem_queries(mem_query_res);
					ang_dfa->extend_rows(mem_query_res);
					is_consistent = false;
				}
				free(mem_query_res);
			}
		}

		vector <symbol_> w_results;
		ang_dfa->update_state_table();
		//cout << "equivalence query for " << ang_dfa->get_num_states() << " states" << endl;
		bool right_dfa = this->ask_equivalence(ang_dfa, w_results);
		//cout << "equivalence query done." << endl;

		if(right_dfa){
			return ang_dfa;
		}
		else{
			list <list<string>> strings_to_verify;
			mem_query_res = new map <list<string>, bool>;
			ang_dfa->get_phrases_from_counterexample(mem_query_res, w_results);
			make_mem_queries(mem_query_res);
			ang_dfa->extend_rows(mem_query_res);
			free(mem_query_res);
		}
	}
}
