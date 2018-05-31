#include <iostream>

#include "concretedfa.h"
#include "angluinlearner.h"
#include "angluindfa.h"
#include "sillyoracle.h"

int main() {

	string file_path;
	cout <<  "inserire percorso file" << endl;
	cin  >> file_path;
	
	ConcreteDfa d1 = d1.read_dfa_file(file_path);;
	d1.read_dfa_file(file_path);
	d1.print_dfa_ttable("real dfa");

	SillyOracle silly(&d1);
	
	vector <string> alphabet;
	alphabet.push_back("a");
	alphabet.push_back("b");

	AngluinLearner ang_learner(&silly, alphabet);
	Dfa* res_dfa = ang_learner.learn();

	//res_dfa->print_dfa_ttable("dfa found");
	d1.print_dfa_ttable("real dfa");
	return 0;
}
