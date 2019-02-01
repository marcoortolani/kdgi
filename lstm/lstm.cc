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
	//std::cout << "get layer output" << std::endl;
	py::object temp = net_->attr("get_layer_output")(layer_, py::cast(net_input));
	std::vector<double> layer_output = temp.cast<std::vector<double>>();
	return layer_output;
}

//LSTMOracle::LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn, py::object* svm){
LSTMOracle::LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn){
	alphabet_ = alphabet;
	net_ = rnn;
	layer = layer_;
	A_ = nullptr;
	
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
	
	build_dfa();
	classifier_.attr("stamp")();
	A_->print_dfa_ttable("_");
	counter_ = 0;
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
	
	/*std::cout << "Dimensione: " << len << std::endl;
	for(auto sym : alphabet_){
		std::cout << "\t" << sym;
	}
	std::cout << std::endl;
	std::cout << std::endl;*/
	
	std::vector<map<symbol_,int>> ttab;
	vector<int> accepting_states;
	
	vector<int> present_states = vector<int>(len, 0);
	vector<int> translate_vec;
	
	for(int i = 0; i < len; i++){
		translate_vec.push_back(i);
		
		ttab.push_back(map<symbol_,int>());
		accepting_states.push_back(membership_query(words_[i]));
		
		for(auto sym : alphabet_){
			auto word = words_[i];
			word.push_back(sym);
			int index = get_state_index_from_word(word);
			ttab.back()[sym] = index;
			
			present_states[index] = 1;
		}
	}
	
	present_states[0] = 1;
	//cout << present_states << endl;
	//cout << translate_vec << endl;
	
	std::vector<map<symbol_,int>> new_ttab;
	for(int i=0; i < len; i++){
		if(present_states[i] == 0){
			for(int j=i+1; j < len; j++){
				translate_vec[j] = translate_vec[j] - 1;
			}
		}
		else{
			new_ttab.push_back(ttab[i]);
		}
	}
	
	len = new_ttab.size();
	for(int i=0; i < len; i++){
		for(auto sym : alphabet_){
			new_ttab[i][sym] = translate_vec[new_ttab[i][sym]];
		}
	}
	
	if(A_ != nullptr){
		delete A_;
	}
	A_ = new ConcreteDfa(len, alphabet_, 0, new_ttab, accepting_states);
	A_->print_dfa_ttable("");
	A_->update_state_table();
	//A_.print_dfa_dot("", "A.dot");
	counter_++;
}
void LSTMOracle::add_word(std::vector<symbol_> word){
	std::vector<double> x_layer;
	words_.push_back(word);
	auto net_input =  vec_sym_to_vec_int(word, alphabet_);
	x_layer = get_layer_output(net_input);
	py::object x = py::cast(x_layer);
	classifier_.attr("add_single_element")(x);
}
