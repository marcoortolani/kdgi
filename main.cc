#include <iostream>

#include "tttdfa.h"
#include "concretedfa.h"

using namespace std;

void close_transitions(TTTDfa& t, ConcreteDfa& c, bool hard_sift){
	vector <bool> queries;
	//auto vec = t.get_transitions_to_sift(hard_sift);

	for(auto vec = t.get_transitions_to_sift(hard_sift); !vec.empty();){
		for (auto pair : vec){
			vector<symbol_> prefix = std :: get<0>(pair);
			prefix.push_back(std :: get<1>(pair));

			queries.clear();
			vector<symbol_> suffix;

			queries.push_back(c.membership_query(prefix));
			//cout << "il prefisso " << prefix << " ha valore " << queries.back() << endl;
			if(t.init_sifting(std :: get<0>(pair), std :: get<1>(pair), suffix, queries.back(), std :: get<2>(pair))){
				do{
					vector <symbol_> phrase = prefix;
					phrase.insert(phrase.end(), suffix.begin(), suffix.end());
					queries.push_back(c.membership_query(phrase));
					//cout << phrase << " ha valore " << queries.back() << endl;

					vector<symbol_> test = {"b", "c", "a"};
				}
				while(t.sift_step(suffix, queries.back(), std :: get<2>(pair)));
			}

			t.close_transition(std :: get<0>(pair), std :: get<1>(pair), queries.back());
		}
		vec = t.get_transitions_to_sift(hard_sift);
	}
	return ;
}

TTTDfa OPACKlearn(ConcreteDfa c){
	c.update_state_table();

	vector<bool> queries;
	for(symbol_ s : c.get_alphabet()){
		queries.push_back(c.membership_query(vector<symbol_>(1, s)));
	}
	queries.push_back(c.membership_query(vector<symbol_>()));

	TTTDfa t(c.get_alphabet(), queries);
	close_transitions(t, c, true);

	t.make_hypotesis();
	////t.print();

	vector<symbol_> counterexample;
	while(!t.equivalence_query(&c, counterexample)){
		bool truth = c.membership_query(counterexample);
		//cout << "counterexample " << counterexample << " is " << truth << endl;

		while(t.membership_query(counterexample) != truth){
			t.handle_counterexample(counterexample);
			////t.print();

			close_transitions(t,c, true);

			////t.print();
			t.make_hypotesis();
		}
	}

	return t;
}


TTTDfa* TTTlearn(ConcreteDfa c){
	c.update_state_table();

	vector<bool> queries;
	for(symbol_ s : c.get_alphabet()){
		queries.push_back(c.membership_query(vector<symbol_>(1, s)));
	}
	queries.push_back(c.membership_query(vector<symbol_>()));

	TTTDfa* t = new TTTDfa(c.get_alphabet(), queries);
	close_transitions(*t, c, false);

	t->make_hypotesis();
	//t.print();

	vector<symbol_> counterexample;
	while(!t->equivalence_query(&c, counterexample)){
		bool truth = c.membership_query(counterexample);
		//cout << "counterexample " << counterexample << " is " << truth << endl;

		while(t->membership_query(counterexample) != truth){
			//cout << "gestione controesempio" << endl;
			t->handle_counterexample(counterexample);
			//t.print();

			//cout << "soft sifting" << endl;
			close_transitions(*t,c, false);

			//t.print();

			vector<vector<symbol_>> prefixes;
			vector<symbol_> suffix;

			while(!t->is_deterministic() && t->try_single_finalization(prefixes, suffix)){
				vector<pair<vector<symbol_>, bool>> leaf_queries;
				for(vector<symbol_> prefix : prefixes){
					vector <symbol_> phrase = prefix;
					phrase.insert(phrase.end(), suffix.begin(), suffix.end());

					leaf_queries.push_back(std :: make_pair(prefix, c.membership_query(phrase)));
				}
				t->split_block(leaf_queries, suffix);
				close_transitions(*t,c,false);
				//cout << "finalization" << endl;
				//t.print();
			}

			if(!t->is_deterministic()){
				//cout << "hard sifting" << endl;
				close_transitions(*t,c, true);
				////t.print();
			}


			//t.print();
			t->make_hypotesis();
		}
	}

	return t;
}

int main() {
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
		TTTDfa op = OPACKlearn(c);
		TTTDfa* t = TTTlearn(c);

		vector<symbol_> counterexample;
		if(!op.equivalence_query(&c, counterexample)){
			cout << "OPack non corrisponde, controesempio trovato: " << counterexample << endl;
		}
		if(!t->equivalence_query(&c, counterexample)){
			cout << "TTTDfa non corrisponde, controesempio trovato: " << counterexample << endl;
		}
	}

	return 0;
}
