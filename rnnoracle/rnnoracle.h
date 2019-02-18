#ifndef LSTM_H
#define LSTM_H

//for python interpreter embedding
#include <pybind11/embed.h>
//for type conversion c++/python
#include<pybind11/stl.h>

#include "concretedfa.h"
#include <iostream>
#include <vector>
#include <string>

namespace py = pybind11;
using namespace py::literals;
using namespace std;

class RNNOracle {
	private:
		//py::object* net_;
		py::object net_;
		py::object classifier_;
		
		std::vector<string> alphabet_;
		int layer_;
		
		std::vector<std::vector<symbol_>> words_;
		ConcreteDfa* A_;
		int counter_;
		int max_build_;
		
		int last_len_;
		map<pair<int, symbol_>, int> mandatory_transitions_;
		
		vector<vector<symbol_>> get_words_by_length(int min_len, int max_len);
		
		int get_state_index_from_word(std::vector<std::string> phrase);
		
		std::vector<double> get_layer_output(std::vector<int> net_input);
		
		bool add_word(std::vector<symbol_> word);
		
		template <class Dfa>
		pair<vector<symbol_>,vector<symbol_>> breadth_search(Dfa* dfa_hp, int max_len);
		
		void build_dfa();
		
	public:
		RNNOracle(std::string filename, std::vector<std::string> alphabet, int layer, int max_build);
		
		bool membership_query(std::vector<std::string> phrase);
		
		template <class Dfa>
		bool equivalence_query(Dfa* dfa_hp , vector<symbol_>& witness_result);
};

#include "rnnoracle.tcc"

#endif
