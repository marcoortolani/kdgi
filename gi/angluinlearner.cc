/*
 * angluinlearner.cpp
 *
 *  Created on: 17/mag/2018
 *      Author: marco
 */

#include "angluinlearner.h"

vector <string> concat_vectors(vector <string> a, vector <string> b){

	if(a.size() == 0 && b.size() == 0){
		return a;
	}

	if(a.size() == 0 && b.size() != 0){
		a = b;
	}
	else{
		if(b.size() != 0){
			a.insert(a.end(), b.begin(), b.end());
		}
	}

	for(int i=a.size()-1; i>=0; i--){
		if(a[i] == ""){
			a.erase(a.begin() + i);
		}
	}

	return a;
}

void AngluinLearner::make_mem_queries(vector<vector<string>> prefixes, vector<vector<string>> suffixes, vector<vector<bool>>* mem_query_res){

	for(unsigned int i=0; i<prefixes.size(); i++){
		vector <bool>new_row(0);
		for(unsigned int j=0; j< suffixes.size(); j++){
			vector <string> query = concat_vectors(prefixes[i], suffixes[j]);
			new_row.push_back(oracle->is_member(query));
		}
		mem_query_res->push_back(new_row);
	}
}

AngluinLearner::AngluinLearner(Oracle* o, vector<string> alphabet):Learner(o, alphabet){};

Dfa* AngluinLearner::learn(){

	cout << "start learning " << endl;

	vector<vector<bool>>* mem_query_res = new vector<vector<bool>>(1, vector<bool>(0));
	vector <string> str;
	for(unsigned int i=0; i<alphabet.size(); i++){
		str = vector<string>(1, alphabet[i]);
		(*mem_query_res)[0].push_back(oracle->is_member(str));
	}
	str = vector <string>(0);
	(*mem_query_res)[0].push_back(oracle->is_member(str));
	ang_dfa = new AngluinDfa(alphabet, (*mem_query_res)[0]);
	free(mem_query_res);

	ang_dfa->print();

	while(true){
		cout << "new cycle" << endl;
		bool is_consistent = false;
		bool is_closed = false;

		while(!is_closed || !is_consistent){

			if(!is_consistent){

				cout << "testing for consistency" << endl;

				vector <vector<string>> prefixes_S = vector <vector<string>>(0);
				vector <vector<string>> prefixes_Sa = vector <vector<string>>(0);
				vector <vector<string>> suffixes = vector <vector<string>>(0);

				is_consistent = ang_dfa->get_inconsistent_string(prefixes_S, prefixes_Sa, suffixes);

				if(!is_consistent){

					cout << "not consistent for string: " << suffixes[0] << endl;

					mem_query_res = new vector<vector<bool>>(0);
					make_mem_queries(prefixes_S, suffixes, mem_query_res);
					make_mem_queries(prefixes_Sa, suffixes, mem_query_res);

					ang_dfa->extend_column(prefixes_S, prefixes_Sa, suffixes, mem_query_res);
					ang_dfa->print();

					free(mem_query_res);

					is_closed = false;

					cout << "insert x" << endl;
					char x;
					cin >> x;
				}
			}

			if(!is_closed){

				cout << "testing for closure" << endl;

				vector<vector<string>> prefixes_S = vector<vector<string>>(0);
				vector<vector<string>> prefixes_Sa = vector<vector<string>>(0);
				vector<vector<string>> suffixes = vector<vector<string>>(0);

				is_closed = ang_dfa->get_open_string(prefixes_S, prefixes_Sa, suffixes);

				if(!is_closed){

					cout << "not closed" << endl;

					mem_query_res = new vector<vector<bool>>(0);
					make_mem_queries(prefixes_S, suffixes, mem_query_res);
					make_mem_queries(prefixes_Sa, suffixes, mem_query_res);

					ang_dfa->extend_row(prefixes_S, prefixes_Sa, mem_query_res);
					ang_dfa->print();

					free(mem_query_res);

					is_consistent = false;

					cout << "insert x" << endl;
					char x;
					cin >> x;
				}
			}
		}

		cout << "equivalence query" << endl;

		Dfa* test_dfa = ang_dfa->gen_dfa();
		test_dfa->print_dfa_ttable("learner test dfa");

		vector <string>* w_results = new vector<string>(0);

		bool right_dfa = oracle->is_equiv(test_dfa, w_results);

		cout << right_dfa << endl;

		if(right_dfa){
			//free(w_results);
			return test_dfa;
		}
		else{

			cout << "wrong dfa" << endl;

			free(test_dfa);
			cout << "because of: \""<< *w_results <<"\""<< endl;

			vector<vector<string>> prefixes_S = vector <vector<string>>(0);
			vector<vector<string>> prefixes_Sa = vector <vector<string>>(0);
			vector<vector<string>> suffixes = vector <vector<string>>(0);

			ang_dfa->get_test(prefixes_S, prefixes_Sa, suffixes, (*w_results));

			//free(w_results);

			mem_query_res = new vector<vector<bool>>(0);
			make_mem_queries(prefixes_S, suffixes, mem_query_res);
			make_mem_queries(prefixes_Sa, suffixes, mem_query_res);

			ang_dfa->extend_row(prefixes_S, prefixes_Sa, mem_query_res);
			ang_dfa->print();

			free(mem_query_res);
			cout << "insert x" << endl;
			char x;
			cin >> x;
		}
	}
}
