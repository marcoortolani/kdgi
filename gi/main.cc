#include <iostream>

#include <map>

#include "angluindfa.h"
#include "angluinlearner.h"
#include "concretedfa.h"
#include "sillyoracle.h"
#include "dfastate.h"

int main() {

	string file_path;
	cout <<  "inserire percorso file" << endl;
	cin  >> file_path;
	
	ConcreteDfa d1;
	d1 = d1.read_dfa_file(file_path);
	SillyOracle silly(&d1);

	vector <string> alphabet;
	alphabet.push_back("a");
	alphabet.push_back("b");

	//TO-DO
	AngluinLearner ang_learner(&silly, alphabet);
	AngluinDfa* res_dfa = ang_learner.learn();
	cout << "dfa della Angluin" << endl;
	res_dfa->print_state_table();


	d1.print_dfa_ttable("dfa originale");
	d1.print_state_table();

	ConcreteDfa d3(d1);
	vector<symbol_> a;
	a.push_back("b");
	a.push_back("a");

	d3.set_alphabet(a);
	d3.print_dfa_ttable("dfa copia del primo con alfabeto invertito");
	d3.print_state_table();

	ConcreteDfa* d4 = res_dfa->gen_concrete_dfa();
	d4->print_dfa_ttable(" concrete dfa generato dalla Angluin");
	d4->print_state_table();

	/*ConcreteDfa d2;
	d2 = d2.read_dfa_file("data/_no_repetitions.txt");
	d2.print_dfa_ttable("dfa completamente diverso");
	d2.print_state_table();*/

	if(d1.is_identical(res_dfa)){
		cout << "d1 è identico a res_dfa" << endl;
	}
	else{
		cout << "d1 è diverso da res_dfa" << endl;
	}

	//cout << d1.is_identical(&d3) << " " << d1.is_identical(&d2) << endl;

	DfaState ds(true, a, map<symbol_, DfaState*>());

	return 0;
}
