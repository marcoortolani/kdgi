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
	bool first = true;
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

//LSTMOracle::LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn, py::object* svm){
LSTMOracle::LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn){
	alphabet_ = alphabet;
	net_ = rnn;
	//classifier_ = svm;
	layer = layer_;
}
	
bool LSTMOracle::membership_query(std::vector<std::string> phrase){
	//std::string s = vec_sym_to_str(phrase, alphabet_);
	std::vector<int> vec_int = vec_sym_to_vec_int(phrase, alphabet_);
	
	//py::print(net_->attr("membership_query")(s));
	//std::cout << "Stringa: " + s << std::endl;
	//double result = net_->attr("membership_query")(s).cast<double>();
	py::object temp = py::cast(vec_int);
	double result = net_->attr("membership_query")(vec_int).cast<double>();
	bool x = result > 0.5 ? true : false;
	return x;
}

void LSTMOracle::build_dfa(std::vector<std::string> phrase){
	//std::string s = vec_sym_to_str(phrase, alphabet_);
	std::vector<int> vec_int = vec_sym_to_vec_int(phrase, alphabet_);
	
	py::object temp = net_->attr("get_layer_output")(layer_, py::cast(vec_int));
	std::vector<double> vec = temp.cast<std::vector<double>>();
	
	for(auto el : vec){
		std::cout << el << std::endl;
	}
}
/*
template<class Dfa>
bool LSTMOracle :: equivalence_query(Dfa* dfa_hp , vector <symbol_>& witness_results){
	return true;
}
*/
