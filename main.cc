#include <iostream>
#include "sillyoracle.h"
#include "randomoracle.h"
#include "tttlearner.h"
#include "angluinlearner.h"
#include "concretedfa.h"

#include "lstm.h"

using namespace std;

template <class Oracle, class Iterator>
void print_results(Oracle* o, Learner<Iterator, Oracle>* l, bool history = false){
	if(l != NULL){
		tuple<int, int, int, int> costs = l->get_costs();
		cout << "\tequivalence cost:" << std :: get<0>(costs);
		cout << "\tmembership cost:" << std :: get<1>(costs);
		cout << "\tsymbol cost:" << std :: get<2>(costs);
		cout << "\tredundant queries:" << std :: get<3>(costs) << endl << endl;
	}
}

template<class I1, class I2, class O>
void compare(Dfa<I1>* d1, Dfa<I2>* d2, O* o, string name1, string name2){
	vector<symbol_> counterexample;
	if(!d1->equivalence_query(d2, counterexample)){
		cout << name1 << " != " << name2 << " because of " << counterexample << endl;
		cout << "structural similarity between " << name1 << " and " << name2 <<  " : ";
		cout << d2->neighbour_matching(d1, true, true)[d2->get_num_states()][0] << " : ";
		cout << d1->neighbour_matching(d2, true, true)[d1->get_num_states()][0] << endl;

		cout << name1 << " : " << d1->membership_query(counterexample) << " ";
		cout << name2 << " : " << d2->membership_query(counterexample) << " ";
		cout << "oracle : " << o->membership_query(counterexample) << endl;
	}
	else{
		cout << name1 << " is equivalent to " << name2;
		if(!d1->is_identical(d2, counterexample)){
			cout << " but not identical" << endl;
		}
		cout << endl;
		cout << "structural similarity between " << name1 << " and " << name2 <<  " : ";
		cout << d2->neighbour_matching(d1, true, true)[d2->get_num_states()][0] << " : ";
		cout << d1->neighbour_matching(d2, true, true)[d1->get_num_states()][0] << endl;
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

		//SillyOracle silly(&c);

		vector<symbol_> counterexample;

		TTTLearner<ConcreteDfa> tttl(&c, c.get_alphabet(), true);
		TTTDfa* op = tttl.learn();
		if(!op->equivalence_query(&c, counterexample)){
			cout << "\tOPack non corrisponde, controesempio trovato: " << counterexample << endl;
		}
		else{
			cout << "\tOPack" << endl;
			print_results(&c, &tttl);
		}
		tttl.reset_costs();

		tttl.set_opack(false);
		TTTDfa* ttt = tttl.learn();
		if(!ttt->equivalence_query(&c, counterexample)){
			cout << "\tTTTDfa non corrisponde, controesempio trovato: " << counterexample << endl;
		}
		else{
			cout << "\tTTTDfa" << endl;
			print_results(&c, &tttl);
		}
		tttl.reset_costs();

		AngluinLearner<ConcreteDfa> al(&c, c.get_alphabet());
		AngluinDfa* ang = al.learn();
		if(!ang->equivalence_query(&c, counterexample)){
			cout << "\tAngluinDfa non corrisponde, controesempio trovato: " << counterexample << endl;
		}
		else{
			cout << "\tAngluinDfa" << endl;
			print_results(&c, &al);
		}
		al.reset_costs();

		compare(ttt, ang, &c, "ttt", "ang");
		compare(ang, ttt, &c, "ang", "ttt");
		compare(ang, ttt, &c, "ang", "ttt");

		/*op->print_ttt_dot("", "dfas/dot/OPack" + name + ".dot");
		ttt->print_ttt_dot("", "dfas/dot/TTT" + name + ".dot");
		ang->print_dfa_dot("", "dfas/dot/ANG" + name + ".dot");*/

		delete op;
		delete ttt;
		delete ang;
	}
}

void test2(int max_length, vector<symbol_> alphabet){
	time_t start;
	time_t end;

	RandomOracle rand(max_length, alphabet);

	vector<symbol_> counterexample;

	cout << "random language generated" << endl << endl;

	TTTLearner<RandomOracle> tttl(&rand, alphabet, true);

	time(&start);
	TTTDfa* op = tttl.learn();
	time(&end);
	cout << "opack time:" << end - start << endl;
	cout << "\tOPack" << endl;
	cout << "\tnum states: " << op->get_num_states() << endl;
	print_results(&rand, &tttl);
	tttl.reset_costs();

	tttl.set_opack(false);
	time(&start);
	TTTDfa* ttt = tttl.learn();
	time(&end);
	cout << "ttt time:" << end - start << endl;

	cout << "\tTTTDfa" << endl;
	cout << "\t num states: " << ttt->get_num_states() << endl;
	print_results(&rand, &tttl);
	tttl.reset_costs();

	AngluinLearner<RandomOracle> al(&rand, alphabet);
	time(&start);
	AngluinDfa* ang = al.learn();
	time(&end);
	cout << "ang time:" << end - start << endl;

	cout << "\tAngluinDfa" << endl;
	cout << "\t num states: " << ang->get_num_states() << endl;
	print_results(&rand, &al);
	al.reset_costs();

	compare(ttt, ang, &rand, "ttt", "ang");
	compare(ang, op, &rand, "ang", "op");
	compare(op, ttt, &rand, "op", "ttt");

	string language = "";
	for(symbol_ sym : alphabet)
		language += sym + "_";

	language += "max_length_" + max_length;

	cout << language << endl;

	/*ttt->print_dfa_dot("ttt_" + language, "dfas/dot/ttt_" + language + ".dot");
	op->print_dfa_dot("opack_" + language, "dfas/dot/opack_" + language + ".dot");
	ang->print_dfa_dot("ang_" + language, "dfas/dot/ang_" + language + ".dot");*/

	delete op;
	delete ttt;
	delete ang;
}

void test3(int max_length){
	vector<vector<symbol_>> dataset;
	string path = "_odd_a_odd_b";
	
	ConcreteDfa c = ConcreteDfa :: read_dfa_file("../gi/data/" + path + ".txt");
	c.update_state_table();
	
	vector<symbol_> alphabet = c.get_alphabet();
	
	//Scorro il numero di simboli che le frasi dovranno avere (da 0 al massimo indicato).
	for(int i = 0; i <= max_length; ++i){
		//Scorro tutti gli interi da 0 ad dim_alfabeto ^ lunghezza_frase_corrente
		//e converto il numero in base dim_alfabeto per poi sostituire ogni cifra al simbolo corrispondente.
		for(int j = 0; j < pow(alphabet.size(), i); ++j){
				int val = j;
				vector<symbol_> word;
				for(int k = 0; k < i; k++){
					word.push_back(alphabet[val % alphabet.size()]);
					val /= alphabet.size();
				}

				cout << word << endl;

				if(c.membership_query(word))
					dataset.push_back(word);
		}
	}
	
	SillyOracle silly(dataset);
	TTTLearner<SillyOracle> l(&silly, c.get_alphabet());
	l.set_opack(false);
	TTTDfa* ttt = l.learn();
	
	ttt->print_dfa_dot("", "dfas/dot/" + path + ".dot");
	
	ttt->print();
	c.print();
	cout << "Results:" << endl;
	cout << ttt->neighbour_matching(&c, true, true)[ttt->get_num_states()][0] << " : ";
	cout << c.neighbour_matching(ttt, true, true)[c.get_num_states()][0] << endl;
	
	delete ttt;
}

void test4(int max_length, int rand_length, vector<symbol_> alphabet){
	RandomOracle rand(rand_length, alphabet);
	vector<vector<symbol_>> dataset;
	
	//Scorro il numero di simboli che le frasi dovranno avere (da 0 al massimo indicato).
	for(int i = 0; i <= max_length; ++i){
		//Scorro tutti gli interi da 0 ad dim_alfabeto ^ lunghezza_frase_corrente
		//e converto il numero in base dim_alfabeto per poi sostituire ogni cifra al simbolo corrispondente.
		for(int j = 0; j < pow(alphabet.size(), i); ++j){
				int val = j;
				vector<symbol_> word;
				for(int k = 0; k < i; k++){
					word.push_back(alphabet[val % alphabet.size()]);
					val /= alphabet.size();
				}

				if(rand.membership_query(word)){
					//cout << word << endl;
					dataset.push_back(word);
				}
		}
	}
	
	SillyOracle silly(dataset);
	
	TTTLearner<RandomOracle> l1(&rand, alphabet);
	TTTLearner<SillyOracle> l2(&silly, alphabet);
	
	TTTDfa* ttt1 = l1.learn();
	cout << "learned l1" << endl;
	TTTDfa* ttt2 = l2.learn();
	cout << "learned l2" << endl;
	
	//ttt1->print_dfa_dot("", "dfas/dot/rand.dot");
	//ttt2->print_dfa_dot("", "dfas/dot/silly.dot");
	
	cout << "Results:" << endl;
	cout << ttt1->neighbour_matching(ttt2, true, true)[ttt1->get_num_states()][0] << " : ";
	cout << ttt2->neighbour_matching(ttt1, true, true)[ttt2->get_num_states()][0] << endl ;
	
	vector<symbol_> counterexample;
	if(! ttt1->equivalence_query(ttt2, counterexample))
		cout << counterexample << endl;
		
	delete ttt1;
	delete ttt2;
}

void test5(){
	vector<symbol_> alphabet = vector<symbol_>{"a", "b"};
	vector<vector<symbol_>> dataset = {vector<symbol_>{"b"},
	vector<symbol_>{"b", "a"},
	vector<symbol_>{"a", "a", "a"},
	vector<symbol_>{"b", "a", "a"},
	vector<symbol_>{"a", "b", "a"},
	vector<symbol_>{"a", "a", "b"},
	vector<symbol_>{"b", "a", "b"},
	vector<symbol_>{"a", "a", "a"},
	vector<symbol_>{"a", "a", "a", "a"},
	vector<symbol_>{"b", "a", "a", "a"},
	vector<symbol_>{"a", "b", "b", "a"},
	vector<symbol_>{"b", "b", "b", "a"},
	vector<symbol_>{"a", "a", "a", "b"},
	vector<symbol_>{"b", "a", "a", "b"},
	vector<symbol_>{"a", "b", "a", "b"},
	vector<symbol_>{"a", "a", "b", "b"},
	vector<symbol_>{"a", "b", "b", "b"},
	vector<symbol_>{"b", "b", "b", "b"},
	vector<symbol_>{"a", "a", "a", "a"},
	vector<symbol_>{"a", "a", "a", "a", "a"},
	vector<symbol_>{"b", "b", "a", "a", "a"},
	vector<symbol_>{"b", "a", "b", "a", "a"},
	vector<symbol_>{"a", "b", "b", "a", "a"},
	vector<symbol_>{"b", "b", "b", "a", "a"},
	vector<symbol_>{"a", "b", "a", "b", "a"},
	vector<symbol_>{"b", "b", "a", "b", "a"},
	vector<symbol_>{"a", "a", "b", "b", "a"},
	vector<symbol_>{"b", "a", "b", "b", "a"},
	vector<symbol_>{"b", "b", "b", "b", "a"},
	vector<symbol_>{"a", "a", "a", "a", "b"},
	vector<symbol_>{"b", "b", "a", "a", "b"},
	vector<symbol_>{"a", "b", "a", "b", "b"},
	vector<symbol_>{"b", "b", "a", "b", "b"},
	vector<symbol_>{"b", "a", "b", "b", "b"},
	vector<symbol_>{"a", "b", "b", "b", "b"},
	vector<symbol_>{"b", "b", "b", "b", "b"},
	vector<symbol_>{"a", "a", "a", "a", "a"},
	vector<symbol_>{"b", "a", "a", "a", "a", "a"},
	vector<symbol_>{"b", "a", "b", "a", "a", "a"},
	vector<symbol_>{"b", "b", "b", "a", "a", "a"},
	vector<symbol_>{"a", "b", "a", "b", "a", "a"},
	vector<symbol_>{"b", "b", "a", "b", "a", "a"},
	vector<symbol_>{"b", "a", "b", "b", "a", "a"},
	vector<symbol_>{"b", "b", "b", "b", "a", "a"},
	vector<symbol_>{"a", "a", "a", "a", "b", "a"},
	vector<symbol_>{"b", "a", "a", "a", "b", "a"},
	vector<symbol_>{"a", "a", "b", "a", "b", "a"},
	vector<symbol_>{"b", "a", "b", "a", "b", "a"},
	vector<symbol_>{"a", "b", "b", "a", "b", "a"},
	vector<symbol_>{"b", "b", "b", "a", "b", "a"},
	vector<symbol_>{"a", "a", "a", "b", "b", "a"},
	vector<symbol_>{"b", "a", "a", "b", "b", "a"},
	vector<symbol_>{"a", "b", "a", "b", "b", "a"},
	vector<symbol_>{"a", "b", "b", "b", "b", "a"},
	vector<symbol_>{"b", "b", "b", "b", "b", "a"},
	vector<symbol_>{"a", "a", "a", "a", "a", "b"},
	vector<symbol_>{"a", "b", "a", "a", "a", "b"},
	vector<symbol_>{"b", "a", "b", "a", "a", "b"},
	vector<symbol_>{"a", "b", "a", "b", "a", "b"},
	vector<symbol_>{"b", "b", "a", "b", "a", "b"},
	vector<symbol_>{"b", "a", "b", "b", "a", "b"},
	vector<symbol_>{"b", "b", "b", "b", "a", "b"},
	vector<symbol_>{"a", "a", "a", "a", "b", "b"},
	vector<symbol_>{"b", "a", "a", "a", "b", "b"},
	vector<symbol_>{"a", "b", "a", "a", "b", "b"},
	vector<symbol_>{"b", "b", "a", "a", "b", "b"},
	vector<symbol_>{"a", "a", "b", "a", "b", "b"},
	vector<symbol_>{"b", "b", "b", "a", "b", "b"},
	vector<symbol_>{"a", "b", "b", "b", "b", "b"},
	vector<symbol_>{"b", "b", "b", "b", "b", "b"}};
	
	SillyOracle silly(dataset);
	TTTLearner<SillyOracle> l2(&silly, alphabet);
	TTTDfa* ttt2 = l2.learn();
	cout << "learned l2" << endl;
	//ttt2->print_dfa_dot("", "dfas/dot/silly.dot");
	
	delete ttt2;
}

void testlstm(){
	py::scoped_interpreter guard{};
	py::module m0 = py::module::import("LSTMOracle");
	py::object net = m0.attr("LSTMOracle")("model6b", 2);
	
	std::vector<std::string> alphabet = {"a", "b"};
	
    LSTMOracle lstm = LSTMOracle(2, alphabet, &net, 10);
    
    //std::cout << lstm.membership_query(std::vector<std::string>{"a", "b", "a", "a"}) << std::endl;
    
    TTTLearner<LSTMOracle> learner(&lstm, alphabet);
	TTTDfa* dfa = learner.learn();
	dfa->print_dfa_dot("", "lstm.dot");
	
	/*
	std::cout << lstm.get_state_index_from_word(std::vector<std::string>()) << std::endl;
	std::cout << lstm.get_state_index_from_word(std::vector<std::string>{"a"}) << std::endl;
	std::cout << lstm.get_state_index_from_word(std::vector<std::string>{"b"}) << std::endl;
	std::cout << lstm.get_state_index_from_word(std::vector<std::string>{"a", "b", "a", "a"}) << std::endl;
	*/
	
	/*
	ConcreteDfa conc = lstm.build_dfa();
	conc.update_state_table();
	
	conc.print_dfa_dot("", "conc.dot");
	vector<symbol_> cnex;
	dfa->equivalence_query(&conc, cnex);
	*/
}

int main() {
	/*
	test1();
	test2(4, vector<symbol_>{"a", "b", "c", "d"});
	test3(8);
	test4(6, 9, vector<symbol_>{"a", "b"});
	test5();
	*/
	
	testlstm();
	/*
	std::vector<map<symbol_,int>> ttab;
	vector<int> accepting_states;
	int len = 4;
	auto alphabet_ = std::vector<symbol_>{"a", "b"};
	for(int i = 0; i < len; i++){
		//std::cout << get_state_index_from_word(words_[i]);
		ttab.push_back(map<symbol_,int>());
		accepting_states.push_back(true);
		
		for(auto sym : alphabet_){
			ttab.back()[sym] = 0;
		}
	}
	
	auto A_ = new ConcreteDfa(len, alphabet_, 0, ttab, accepting_states);
	delete A_;
	A_ = new ConcreteDfa(len, alphabet_, 0, ttab, accepting_states);
	A_->print_dfa_ttable("");
	A_->print_state_table();
	for(int i=0; i < 100; i++){
		std::cout << i << std::endl;
		delete A_;
		A_ = new ConcreteDfa(len, alphabet_, 0, ttab, accepting_states);
	}
	*/
}
