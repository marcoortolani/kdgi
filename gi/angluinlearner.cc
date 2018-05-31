#include "angluinlearner.h"
#include "concretedfa.h"
#include <iostream>

using namespace std;

void AngluinLearner::make_mem_queries(vector<vector<string>> phrases_to_verify, vector<bool>* mem_query_res){

	for(unsigned int i=0; i<phrases_to_verify.size(); i++){
		mem_query_res->push_back(oracle->is_member(phrases_to_verify[i]));
	}
}

AngluinLearner::AngluinLearner(Oracle* o, vector<string> alphabet):Learner(o, alphabet){
	ang_dfa = NULL;
};

Dfa* AngluinLearner::learn(){

	cout << "start learning " << endl;

	vector<bool>* mem_query_res = new vector<bool>(0);

	vector <string> str;

	for(unsigned int i=0; i<alphabet.size(); i++){
		str = vector<string>(1, alphabet[i]);
		mem_query_res->push_back(oracle->is_member(str));
	}
	
	str = vector <string>(0);
	mem_query_res->push_back(oracle->is_member(str));
	
	ang_dfa = new AngluinDfa(alphabet, (*mem_query_res));
	free(mem_query_res);
	
	ang_dfa->print();

	while(true){
		cout << "new cycle" << endl;
		bool is_consistent = false;
		bool is_closed = false;

		while(!is_closed || !is_consistent){

			if(!is_consistent){

				cout << "testing for consistency" << endl;

				vector <vector<string>> strings_to_verify = vector<vector<string>>(0);

				is_consistent = ang_dfa->get_inconsistent_phrases(strings_to_verify);
				if(!is_consistent){

					cout << "got inconsistent strings." << endl;

					mem_query_res = new vector<bool>(0);
					make_mem_queries(strings_to_verify, mem_query_res);

					cout << "queries done." << endl;
					for(unsigned int i=0; i<strings_to_verify.size(); i++){
						cout << "string:" << strings_to_verify[i] << " res:" << (*mem_query_res)[i] << endl;
					}

					ang_dfa->extend_column(strings_to_verify, mem_query_res);
					free(mem_query_res);

					cout << "column extended." << endl;
					ang_dfa->print();

					is_closed = false;

					cout << "continue" << endl;
					char x;
					cin >> x;
				}
			}

			if(!is_closed){

				cout << "testing for closure" << endl;

				vector <vector<string>> strings_to_verify = vector<vector<string>>(0);

				is_closed = ang_dfa->get_open_phrases(strings_to_verify);

				if(!is_closed){

					cout << "got open strings." << endl;

					mem_query_res = new vector<bool>(0);
					make_mem_queries(strings_to_verify, mem_query_res);

					cout << "queries done." << endl;
					for(unsigned int i=0; i<strings_to_verify.size(); i++){
						cout << "string:" << strings_to_verify[i] << " res:" << (*mem_query_res)[i] << endl;
					}

					ang_dfa->extend_rows(strings_to_verify, mem_query_res);
					free(mem_query_res);

					cout << "rows extended." << endl;
					ang_dfa->print();

					is_consistent = false;

					cout << "continue" << endl;
					char x;
					cin >> x;
				}
			}
		}

		cout << "equivalence query" << endl;

		ConcreteDfa* test_dfa = ang_dfa->gen_concrete_dfa();
		test_dfa->print_dfa_ttable("learner test dfa");

		vector <string>* w_results = new vector<string>(0);

		bool right_dfa = oracle->is_equiv(test_dfa, w_results);

		if(right_dfa){
			return test_dfa;
		}
		else{

			cout << "wrong dfa" << endl;

			free(test_dfa);
			cout << "because of: \""<< *w_results <<"\""<< endl;

			vector <vector<string>> strings_to_verify = vector<vector<string>>(0);

			ang_dfa->get_phrases_from_counterexample(strings_to_verify, (*w_results));
			cout << "test done" << endl;

			mem_query_res = new vector<bool>(0);
			make_mem_queries(strings_to_verify, mem_query_res);

			cout << "queries done." << endl;
			for(unsigned int i=0; i<strings_to_verify.size(); i++){
				cout << "string:" << strings_to_verify[i] << " res:" << (*mem_query_res)[i] << endl;
			}

			ang_dfa->extend_rows(strings_to_verify, mem_query_res);
			free(mem_query_res);

			cout << "rows extended." << endl;
			ang_dfa->print();

			cout << "continue" << endl;
			char x;
			cin >> x;
		}
	}
}
