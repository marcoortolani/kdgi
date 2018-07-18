#include "angluinlearner.h"

#include "concretedfa.h"
#include <iostream>

using namespace std;


void AngluinLearner::make_mem_queries(map<list<string>, bool>* mem_query_res){
	for(auto pair : *mem_query_res){
		vector<string> vec;
		vec.insert(vec.end(), pair.first.begin(), pair.first.end());
		(*mem_query_res)[pair.first] = (oracle->is_member(vec));
	}
}


AngluinLearner::AngluinLearner(Oracle* o, vector<string> a) : Learner(o, a){
	ang_dfa = NULL;
};


AngluinDfa* AngluinLearner::learn(){
	//cout << "start learning " << endl;
	map <list<string>, bool>* mem_query_res = new map<list<string>, bool>;
	//TO-DO: cercare di risolvere questa "ridondanza"
	vector <symbol_> str_vec;
	list <symbol_> str_lis;
	for(unsigned int i=0; i<alphabet.size(); i++){
		str_vec = vector<string>(1, alphabet[i]);
		str_lis = list<string>(1, alphabet[i]);
		(*mem_query_res)[str_lis] = oracle->is_member(str_vec);
	}
	str_vec = vector <string>(0);
	str_lis = list<symbol_>(1, "");
	(*mem_query_res)[str_lis] = oracle->is_member(str_vec);
	ang_dfa = new AngluinDfa(alphabet, mem_query_res);
	delete(mem_query_res);
	//ang_dfa->print();

	while(true){
		//cout << "new cycle" << endl;
		bool is_consistent = false;
		bool is_closed = false;

		while(!is_closed || !is_consistent){
			if(!is_consistent){
				//cout << "testing for consistency" << endl;
				mem_query_res = new map<list<string>, bool>;
				is_consistent = ang_dfa->get_inconsistent_phrases(mem_query_res);
				if(!is_consistent){
					//cout << "got inconsistent strings." << endl;
					make_mem_queries(mem_query_res);
					//cout << "queries done." << endl;
					ang_dfa->extend_column(mem_query_res);
					//cout << "column extended." << endl;
					//ang_dfa->print();
					is_closed = false;
					//cout << "continue" << endl;
					//char x;
					//cin >> x;
				}
				free(mem_query_res);
			}

			if(!is_closed){
				//cout << "testing for closure" << endl;
				mem_query_res = new map<list<string>, bool>;
				is_closed = ang_dfa->get_open_phrases(mem_query_res);
				if(!is_closed){
					//cout << "got open strings." << endl;
					make_mem_queries(mem_query_res);
					//cout << "queries done." << endl;
					ang_dfa->extend_rows(mem_query_res);
					//cout << "rows extended." << endl;
					//ang_dfa->print();
					is_consistent = false;
					//cout << "continue" << endl;
					//char x;
					//cin >> x;
				}
				free(mem_query_res);
			}
		}

		//cout << "equivalence query" << endl;
		ConcreteDfa* test_dfa = ang_dfa->gen_concrete_dfa();
		//test_dfa->print_dfa_ttable("learner test dfa");
		vector <string>* w_results = new vector<string>(0);
		bool right_dfa = oracle->is_equiv(test_dfa, w_results);

		if(right_dfa){
			return ang_dfa;
		}
		else{
			//cout << "wrong dfa" << endl;
			free(test_dfa);
			//cout << "because of: \""<< *w_results <<"\""<< endl;
			list <list<string>> strings_to_verify;
			mem_query_res = new map <list<string>, bool>;
			ang_dfa->get_phrases_from_counterexample(mem_query_res, (*w_results));
			//cout << "test done" << endl;
			make_mem_queries(mem_query_res);
			//cout << "queries done." << endl;
			ang_dfa->extend_rows(mem_query_res);
			free(mem_query_res);
			//cout << "rows extended." << endl;
			//ang_dfa->print();
			//cout << "continue" << endl;
			//char x;
			//cin >> x;
		}
	}
}
