#include <iostream>

#include <map>
#include <deque>

#include "angluindfa.h"
#include "angluinlearner.h"
#include "concretedfa.h"
#include "sillyoracle.h"
#include "dfastate.h"

int main() {

	vector<string> file_names = {	//"tomita9",
									//"tomita15",
									"_all",
									"_2_instance_of_aab",
									"_ab_aaaa",
									"_all_except_abab",
									"_no_repetitions",
									"_odd_a_odd_b",
									"dfa_test"
									};

	vector<ConcreteDfa> Dfas;
	vector<AngluinDfa*> AngDfas;
	for(auto name : file_names){
		ConcreteDfa d1 = d1.read_dfa_file("../gi/data/" + name + ".txt");
		Dfas.push_back(d1);

		SillyOracle o(&d1);
		AngluinLearner l(&o, d1.sort_alphabet());
		AngDfas.push_back(l.learn());
	}

	int i = 0;
	for(ConcreteDfa d : Dfas){
		int j = 0;
		for(AngluinDfa* a : AngDfas){
			vector<symbol_> vec;
			if(! d.Dfa::equivalence_query(a, vec)){
				cout << file_names[i] << " " << file_names[j] << endl;
				cout << "\t" << vec << endl;
				if(d[vec]->is_accepting() == (*a)[vec]->is_accepting()){
					cout << "\terrore qui" << endl;
				}
				cout << "\td:" << d[vec]->is_accepting() << " a:" << (*a)[vec]->is_accepting() << endl;
			}
			++j;
		}
		++i;
	}

	return 0;
}
