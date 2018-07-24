#include <iostream>

#include <map>
#include <deque>

#include "angluindfa.h"
#include "angluinlearner.h"
#include "concretedfa.h"
#include "sillyoracle.h"
#include "dfastate.h"

int main() {

	vector<string> file_names = {"_2_instance_of_aab", "_ab_aaaa", "_all_except_abab", "_no_repetitions", "_odd_a_odd_b", "dfa_test"};
	vector<ConcreteDfa> Dfas;
	for(auto name : file_names){
		ConcreteDfa d1 = d1.read_dfa_file("../gi/data/" + name + ".txt");
		Dfas.push_back(d1);
	}

	for(auto dfa1 : Dfas){
		for(auto dfa2 : Dfas){
			vector<symbol_> phrase;
			bool test = dfa1.is_identical(&dfa2, phrase);
			if(test){
				cout << "1";
			}
			else{
				cout << phrase;
			}
			cout << "\t";
		}
		cout << endl;
	}

	DfaState ds = *(Dfas.front().begin());

	deque<symbol_>{"a", "b", "a"};
	ds[vector<symbol_>{"a", "b", "a"}];								//compila
	ds[list<symbol_>{"a", "b", "a"}];								//compila
	ds[deque<symbol_>{"a", "b", "a"}];								//compila

	//ds[vector<int>{1,2,3}];										//non compila


	return 0;
}
