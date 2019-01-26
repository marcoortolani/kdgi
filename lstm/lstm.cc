#include "lstm.h"

namespace py = pybind11;
using namespace py::literals;
using namespace std;
/*
std::string vec_sym_to_str(std::vector<std::string> phrase, std::vector<std::string> alphabet){
	std::string s;
	bool first = true;
	for(std::string sym : phrase){
		bool unidentified_symbol = true;
		if(!first){
				s += " ";
			}
			else{
				first = false;
			}
			
		for(int i = 0; i < alphabet.size(); i++){
			if(sym.compare(alphabet[i]) == 0){
				unidentified_symbol = false;
				s += std::to_string(i + 1);
			}
		}
		if(unidentified_symbol){
			std::cerr << "Error in LSTMOracle::vec_sym_to_str" << std::endl;
			throw 0;
		}
	}
	
	if(first){
		s = "0";
	}
	return s;
}
*/

std::vector<int> vec_sym_to_vec_int(std::vector<std::string> phrase, std::vector<std::string> alphabet){
	std::vector<int> vec_int;
	for(std::string sym : phrase){
		bool unidentified_symbol = true;
		for(int i = 0; i < alphabet.size(); i++){
			if(sym.compare(alphabet[i]) == 0){
				unidentified_symbol = false;
				vec_int.push_back(i + 1);
			}
		}
		if(unidentified_symbol){
			std::cerr << "Error in LSTMOracle::vec_sym_to_str" << std::endl;
			throw 0;
		}
	}
	
	return vec_int;
}

std::vector<double> LSTMOracle::get_layer_output(std::vector<int> net_input){
	py::object temp = net_->attr("get_layer_output")(layer_, py::cast(net_input));
	std::vector<double> layer_output = temp.cast<std::vector<double>>();
	return layer_output;
}

//LSTMOracle::LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn, py::object* svm){
LSTMOracle::LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn){
	alphabet_ = alphabet;
	net_ = rnn;
	layer = layer_;
	
	std::vector<std::vector<double>> X_layer;
	words_.push_back(std::vector<std::string>());
	auto net_input =  vec_sym_to_vec_int(words_.back(), alphabet_);
	X_layer.push_back(get_layer_output(net_input));
	
	for(auto sym : alphabet_){
		words_.push_back(std::vector<std::string>{sym});
		net_input =  vec_sym_to_vec_int(words_.back(), alphabet_);
		X_layer.push_back(get_layer_output(net_input));
	}
	
	py::module svm_module = py::module::import("svm");
	
	
	py::object X = py::cast(X_layer);
	classifier_ = svm_module.attr("SVMClassifier")(X);
}
	
bool LSTMOracle::membership_query(std::vector<std::string> phrase){
	std::vector<int> vec_int = vec_sym_to_vec_int(phrase, alphabet_);
	py::object temp = py::cast(vec_int);
	double result = net_->attr("membership_query")(vec_int).cast<double>();
	bool x = result > 0.5 ? true : false;
	return x;
}

int LSTMOracle::get_state_index_from_word(std::vector<std::string> phrase){
	std::vector<int> net_input = vec_sym_to_vec_int(phrase, alphabet_);
	std::vector<double> layer_output = get_layer_output(net_input);
	py::object temp = classifier_.attr("get_index")(py::cast(layer_output));
	int index = temp.cast<int>();
	return index;
}

void LSTMOracle::build_dfa(){
	int len = classifier_.attr("get_dim")().cast<int>();
	std::cout << "Dimensione: " << len << std::endl;
	for(auto sym : alphabet_){
		std::cout << "\t" << sym;
	}
	std::cout << std::endl;
	std::cout << std::endl;
	
	for(int i = 0; i < len; i++){
		std::cout << get_state_index_from_word(words_[i]);
		for(auto sym : alphabet_){
			auto word = words_[i];
			word.push_back(sym);
			std::cout << "\t" << get_state_index_from_word(word);
		}
		std::cout << std::endl;
	}
}
