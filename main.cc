#include <iostream>

#include "sillyoracle.h"
#include "randomoracle.h"
#include "tttlearner.h"
#include "angluinlearner.h"
#include "concretedfa.h"

using namespace std;

template <class Oracle>
void print_results(Oracle* o, bool history = false){
	/*tuple<int, int, int, int> costs = o->get_costs();
	cout << "\tequivalence cost:" << std :: get<0>(costs);
	cout << "\tmembership cost:" << std :: get<1>(costs);
	cout << "\tsymbol cost:" << std :: get<2>(costs);
	cout << "\tredundant queries:" << std :: get<3>(costs) << endl << endl;

	if(history){
		vector<pair<vector<symbol_>, bool>> query_history = o->get_query_history();
		for(auto pair : query_history)
			cout << "\t"<< pair.first << " : " << pair.second << endl;

		cout << endl;
	}*/
}

template<class I1, class I2, class O>
void compare(Dfa<I1>* d1, Dfa<I2>* d2, O* o, string name1, string name2){
	vector<symbol_> counterexample;
	if(!d1->equivalence_query(d2, counterexample)){
		cout << name1 << " != " << name2 << " because of " << counterexample << endl;
		cout << "structural similarity between ttt and ang : ";
		//cout << d1->neighbour_matching(d2, true, true)[d1->get_num_states()][0] << endl;

		cout << name1 << " : " << d1->membership_query(counterexample) << " ";
		cout << name2 << " : " << d2->membership_query(counterexample) << " ";
		cout << "oracle : " << o->membership_query(counterexample) << endl;
	}
	else{
		cout << name1 << " is equivalent to " << name2;
		if(!d1->equivalence_query(d2, counterexample)){
			cout << " but not identical" << endl;
		}
		cout << endl;
	}
	cout << endl;
}

void test1(){
	vector<string> files = {"_2_instance_of_aab",
							"_ab_aaaa",
							"_all",
							"_no_repetitions",
							"_odd_a_odd_b",
							"_only_a",
							"dfa_test",
							"_all_except_abab",
							"tomita15",
							"tomita9"
							};

	for(string name : files){
		string path = "../gi/data/" + name + ".txt";
		cout << name << endl;

		ConcreteDfa c = c.read_dfa_file(path);
		c.update_state_table();

		SillyOracle silly(&c);


		vector<symbol_> counterexample;

		TTTLearner<SillyOracle> tttl(&silly, c.get_alphabet(), true);
		TTTDfa* op = tttl.learn();
		if(!op->equivalence_query(&c, counterexample)){
			cout << "\tOPack non corrisponde, controesempio trovato: " << counterexample << endl;
		}
		else{
			cout << "\tOPack" << endl;
			print_results(&silly);
		}
		//silly.reset();

		tttl.set_opack(false);
		TTTDfa* ttt = tttl.learn();
		if(!ttt->equivalence_query(&c, counterexample)){
			cout << "\tTTTDfa non corrisponde, controesempio trovato: " << counterexample << endl;
		}
		else{
			cout << "\tTTTDfa" << endl;
			print_results(&silly);
		}
		//silly.reset();

		AngluinLearner<SillyOracle> al(&silly, c.get_alphabet());
		AngluinDfa* ang = al.learn();
		if(!ang->equivalence_query(&c, counterexample)){
			cout << "\tAngluinDfa non corrisponde, controesempio trovato: " << counterexample << endl;
		}
		else{
			cout << "\tAngluinDfa" << endl;
			print_results(&silly);
		}

		compare(ttt, ang, &silly, "ttt", "ang");
		compare(ang, ttt, &silly, "ang", "ttt");
		compare(ang, ttt, &silly, "ang", "ttt");

		//op->print_dfa_dot("", "dfas/dot/OPack" + name + ".dot");
		//ttt->print_ttt_dot("", "dfas/dot/TTT" + name + ".dot");
		//ang->print_dfa_dot("", "dfas/dot/ANG" + name + ".dot");

		delete op;
		delete ttt;
		delete ang;
	}
}

void test2(){
	time_t start;
	time_t end;


	int max_length = 3;
		vector<symbol_> alphabet = {"a", "b", "c", "d"};
		RandomOracle rand(max_length, alphabet);

		vector<symbol_> counterexample;

		cout << "random language generated" << endl << endl;

		TTTLearner<RandomOracle> tttl(&rand, alphabet, true);

		TTTDfa* op = tttl.learn();
		cout << "\tOPack" << endl;
		cout << "\tnum states: " << op->get_num_states() << endl;
		print_results(&rand);
		//rand.reset();

		//op->Dfa::print();

		tttl.set_opack(false);
		time(&start);
		TTTDfa* ttt = tttl.learn();
		time(&end);
		cout << "ttt time:" << end - start << endl;

		cout << "\tTTTDfa" << endl;
		cout << "\t num states: " << ttt->get_num_states() << endl;
		print_results(&rand);
		//rand.reset();

		//ttt->Dfa::print();

		AngluinLearner<RandomOracle> al(&rand, alphabet);
		time(&start);
		AngluinDfa* ang = al.learn();
		time(&end);
		cout << "ang time:" << end - start << endl;

		cout << "\tAngluinDfa" << endl;
		cout << "\t num states: " << ang->get_num_states() << endl;
		print_results(&rand);

		//ang->Dfa::print();

		compare(ttt, ang, &rand, "ttt", "ang");
		compare(ang, op, &rand, "ang", "op");
		compare(op, ttt, &rand, "op", "ttt");

		string language = "";
		for(symbol_ sym : alphabet)
			language += sym + "_";

		language += "max_length_" + max_length;

		cout << language << endl;

		//ttt->print_dfa_dot("ttt_" + language, "dfas/dot/ttt_" + language + ".dot");
		//op->print_dfa_dot("opack_" + language, "dfas/dot/opack_" + language + ".dot");
		//ang->print_dfa_dot("ang_" + language, "dfas/dot/ang_" + language + ".dot");

		delete op;
		delete ttt;
		delete ang;
}

int main() {
	test1();
	test2();
}
