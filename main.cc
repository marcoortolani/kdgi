#include <iostream>

#include <map>
#include <deque>

#include "angluindfa.h"
#include "angluinlearner.h"
#include "concretedfa.h"
#include "sillyoracle.h"
#include "dfastate.h"

int main() {

	vector<string> file_names = {	"tomita9",
									"tomita15",
									//"_2_instance_of_aab",
									//"_ab_aaaa",
									//"_all_except_abab",
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

	/*int m, n;
	while(true){
		cout << "Inserisci m" << endl;
		cin >> m;
		cout << "Inserisci n" << endl;
		cin >> n;

		cout << "prova " << file_names[m] << " && " << file_names[n] << endl;
		vector<vector<double>> sim = Dfas[m].neighbour_matching(&Dfas[n]);
		double global_sim = (sim.back()).front();
		cout << global_sim << endl;
		cout << file_names[m] << " " << file_names[n] << " ok." << endl;
	}*/


	cout << endl;
	for(auto dfa1 : Dfas){
		for(auto dfa2 : Dfas){
			vector<vector<double>> sim = dfa1.neighbour_matching(&dfa2);
			double global_sim = (sim.back()).front();
			cout << global_sim << "          ";
		}
		cout << endl;
	}

	cout << endl;
	for(auto dfa1 : AngDfas){
		for(auto dfa2 : AngDfas){
			vector<vector<double>> sim = dfa1->neighbour_matching(dfa2);
			double global_sim = (sim.back()).front();
			cout << global_sim << "          ";
		}
		cout << endl;
	}

	cout << endl;
	for(auto dfa1 : Dfas){
		for(auto dfa2 : AngDfas){
			vector<vector<double>> sim = dfa1.neighbour_matching(dfa2);
			double global_sim = (sim.back()).front();
			cout << global_sim << "          ";
		}
		cout << endl;
	}

	cout << endl;
	for(auto dfa1 : AngDfas){
		for(auto dfa2 : Dfas){
			vector<vector<double>> sim = dfa1->neighbour_matching(&dfa2);
			double global_sim = (sim.back()).front();
			cout << global_sim << "          ";
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
