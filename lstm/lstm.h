#ifndef LSTM_H
#define LSTM_H

//for python interpreter embedding
#include <pybind11/embed.h>
//for type conversion c++/python
#include<pybind11/stl.h>

//#include "ConcreteDfa.h"
#include <iostream>
#include <vector>
#include <string>

namespace py = pybind11;
using namespace py::literals;
using namespace std;

class LSTMOracle {
	private:
		py::object* net_;
		py::object classifier_;
		
		std::vector<string> alphabet_;
		int layer_;
		
		std::vector<std::vector<std::string>> words_;
		
	public:
		//private methods made temporarily public for debugging purpouse
		int get_state_index_from_word(std::vector<std::string> phrase);
		std::vector<double> get_layer_output(std::vector<int> net_input);
		void build_dfa();
		
		//real public methods
		//LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn, py::object* svm);
		LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn);
		
		bool membership_query(std::vector<std::string> phrase);
		
		template <class Dfa>
		bool equivalence_query(Dfa* dfa_hp , vector<string>& witness_results){return true;};
};

#endif
