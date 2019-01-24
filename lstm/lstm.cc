#include "lstm.h"

namespace py = pybind11;
using namespace py::literals;
using namespace std;

LSTMOracle::LSTMOracle(std::string modelname, int layer, std::vector<std::string> alphabet, py::object* n){
	net_ = n;
	alphabet_ = alphabet;
}
	
bool LSTMOracle::membership_query(std::vector<std::string> phrase){
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
			
		for(int i = 0; i < alphabet_.size(); i++){
			if(sym.compare(alphabet_[i]) == 0){
				unidentified_symbol = false;
				s += std::to_string(i + 1);
			}
		}
		if(unidentified_symbol){
			std::cerr << "Error in LSTMOracle::membership_query" << std::endl;
			throw 0;
		}
	}
	
	if(first){
		s = "0";
	}
	
	//py::print(net_->attr("membership_query")(s));
	std::cout << "Stringa: " + s << std::endl;
	double result = net_->attr("membership_query")(s).cast<double>();
	bool x = result > 0.5 ? true : false;
	return x;
}
/*
template<class Dfa>
bool LSTMOracle :: equivalence_query(Dfa* dfa_hp , vector <symbol_>& witness_results){
	return true;
}
*/
