#include <cmath>

namespace py = pybind11;
using namespace py::literals;
using namespace std;

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

vector<vector<symbol_>> LSTMOracle::get_words_by_length(int min_len, int max_len){
	vector<vector<symbol_>> words;
	int len = alphabet_.size();
	
	for(int i = min_len; i < max_len; i++){
		int c = pow(len, i);
		for(int j = 0; j < c; j++){
			int val = j;
			vector<symbol_> w;
			for(int k = 0; k < i; k++){
				int q = val % len;
				val = val / len;
				w.push_back(alphabet_[q]);
			}
			vector<symbol_> word;
			for(int l = w.size() - 1; l >= 0; l--){
				word.push_back(w[l]);
			}
			words.push_back(word);
		}
	}
	
	return words;
}

std::vector<double> LSTMOracle::get_layer_output(std::vector<int> net_input){
	py::object temp = net_->attr("get_layer_output")(layer_, py::cast(net_input));
	std::vector<double> layer_output = temp.cast<std::vector<double>>();
	return layer_output;
}

int LSTMOracle::get_state_index_from_word(std::vector<std::string> phrase){
	std::vector<int> net_input = vec_sym_to_vec_int(phrase, alphabet_);
	std::vector<double> layer_output = get_layer_output(net_input);
	py::object temp = classifier_.attr("get_index")(py::cast(layer_output));
	int index = temp.cast<int>();
	return index;
}

void LSTMOracle::add_word(std::vector<symbol_> word){
	for(auto w : words_){
		if(w == word){
			cout << "A_: " << A_->membership_query(word) << endl;
			cout << "classifier: " << membership_query(word) << endl;
		}
	}
	
	vector<symbol_> prefix = word;
	symbol_ transiction = prefix.back();
	prefix.pop_back();
	
	int start_state = A_->get_arrive_state(prefix);
	int arrive_state = words_.size();
	mandatory_transictions_[make_pair(start_state, transiction)] = arrive_state;
	
	cout << "\tnew word: " << word << endl;
	std::vector<double> x_layer;
	words_.push_back(word);
	auto net_input =  vec_sym_to_vec_int(word, alphabet_);
	x_layer = get_layer_output(net_input);
	py::object x = py::cast(x_layer);
	classifier_.attr("add_single_element")(x);
}

template <class Dfa>
pair<vector<symbol_>,vector<symbol_>> LSTMOracle::breadth_search(Dfa* dfa_hp, int min_len, int max_len){
	vector<vector<symbol_>> words_by_length = get_words_by_length(min_len, max_len);
	map<vector<symbol_>, pair<vector<symbol_>, vector<symbol_>>> state_recorder;
	
	for(auto word : words_by_length){
		DfaState* lstmState = (*A_)[word];
		DfaState* learnerState = (*dfa_hp)[word];
		auto it = state_recorder.find(lstmState->get_charact_phrase());
		if(it == state_recorder.end()){
			state_recorder[lstmState->get_charact_phrase()] = make_pair(learnerState->get_charact_phrase(), word);
		}
		else{
			if(it->second.first != learnerState->get_charact_phrase()){
				return make_pair(it->second.second, word);
			}
		}
	}
	
	return make_pair(vector<symbol_>(), vector<symbol_>());
}


template <class Dfa>
pair<vector<symbol_>,vector<symbol_>> LSTMOracle::breadth_search2(Dfa* dfa_hp, int max_len){
	
	int len = alphabet_.size();
	map<vector<symbol_>, pair<vector<symbol_>, vector<symbol_>>> state_recorder;
	
	for(int i = 0; i < max_len; i++){
		int c = pow(len, i);
		for(int j = 0; j < c; j++){
			int val = j;
			vector<symbol_> w;
			for(int k = 0; k < i; k++){
				int q = val % len;
				val = val / len;
				w.push_back(alphabet_[q]);
			}
			vector<symbol_> word;
			for(int l = w.size() - 1; l >= 0; l--){
				word.push_back(w[l]);
			}
			
			DfaState* lstmState = (*A_)[word];
			DfaState* learnerState = (*dfa_hp)[word];
			auto it = state_recorder.find(lstmState->get_charact_phrase());
			if(it == state_recorder.end()){
				state_recorder[lstmState->get_charact_phrase()] = make_pair(learnerState->get_charact_phrase(), word);
			}
			else{
				if(it->second.first != learnerState->get_charact_phrase()){
					return make_pair(it->second.second, word);
				}
			}
		}
	}
	
	return make_pair(vector<symbol_>(), vector<symbol_>());
}

void LSTMOracle::build_dfa(){
	int len = classifier_.attr("get_dim")().cast<int>();
	
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
			
			auto it = mandatory_transictions_.find(make_pair(i, sym));
			if(it == mandatory_transictions_.end()){
				int index = get_state_index_from_word(word);
				ttab.back()[sym] = index;
			
				present_states[index] = 1;
				//cout << "not in_words" << endl;
			}
			else{
				ttab.back()[sym] = it->second;
				present_states[it->second] = 1;
				//cout << "in_words" << endl;
			}
			/*
			bool in_words = false;
			for(int j=0; j<len and !in_words; j++){
				if(word == words_[j]){
					in_words = true;
					ttab.back()[sym] = j;
					present_states[j] = 1;
					//cout << "in_words" << endl;
				}
			}
			

			if(!in_words){
				int index = get_state_index_from_word(word);
				ttab.back()[sym] = index;
			
				present_states[index] = 1;
				//cout << "not in_words" << endl;
			}
			*/
			//cout << word << ":" << sym << ":" << ttab.back()[sym] << endl;
		}
	}
	
	/*
	present_states[0] = 1;
	
	std::vector<map<symbol_,int>> new_ttab;
	vector<int> new_accepting_states;
	for(int i=0; i < len; i++){
		if(present_states[i] == 0){
			for(int j=i+1; j < len; j++){
				translate_vec[j] = translate_vec[j] - 1;
			}
		}
		else{
			new_ttab.push_back(ttab[i]);
			new_accepting_states.push_back(accepting_states[i]);
		}
	}
	
	len = new_ttab.size();
	for(int i=0; i < len; i++){
		for(auto sym : alphabet_){
			new_ttab[i][sym] = translate_vec[new_ttab[i][sym]];
		}
	}
	*/
	if(A_ != nullptr){
		delete A_;
	}
	//A_ = new ConcreteDfa(len, alphabet_, 0, new_ttab, new_accepting_states);
	A_ = new ConcreteDfa(len, alphabet_, 0, ttab, accepting_states);
	A_->print_dfa_ttable("A_");
	A_->update_state_table();
	cout << "counter: " << counter_++ << endl;
}

LSTMOracle::LSTMOracle(int layer, std::vector<std::string> alphabet, py::object* rnn, int max_build){
	alphabet_ = alphabet;
	net_ = rnn;
	layer = layer_;
	A_ = nullptr;
	max_build_ = max_build;
	
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
	
	counter_ = 0;
	last_len_ = 0;
}

bool LSTMOracle::membership_query(std::vector<std::string> phrase){
	std::vector<int> vec_int = vec_sym_to_vec_int(phrase, alphabet_);
	py::object temp = py::cast(vec_int);
	double result = net_->attr("membership_query")(vec_int).cast<double>();
	bool x = result > 0.5 ? true : false;
	return x;
}

template <class Dfa>
bool LSTMOracle::equivalence_query(Dfa* dfa_hp , vector<symbol_>& witness_result){
	cout << counter_ << ":" << max_build_ << endl;
	while(counter_ < max_build_){
		build_dfa();
		
		vector<symbol_> cntr;
		if(A_->equivalence_query(dfa_hp, cntr)){
			return true;
		}
		else{
			cout << "cntr: " << cntr << endl;
		}
		
		auto word_pair = breadth_search2(dfa_hp, A_->get_num_states());
		
		vector<symbol_> prefix1 = word_pair.first;
		vector<symbol_> prefix2 = word_pair.second;
		
		if(prefix1.empty() and prefix2.empty()){
			return true;
		}
		
		last_len_ = prefix1.size() < prefix2.size() ? prefix1.size() - 1 : prefix2.size() - 1;
		
		vector<symbol_> phrase1 = (*dfa_hp)[prefix1]->get_charact_phrase();
		vector<symbol_> phrase2 = (*dfa_hp)[prefix2]->get_charact_phrase();
		vector<symbol_> suffix;
		
		if(dfa_hp->equivalence_query(dfa_hp, suffix, phrase1, phrase2)){
			cerr << "Error in LSTMOracle::equivalence_query: 2 different states of the hypothesis cannot be equivalent! Use a canonical hypothesis." << endl;
			cerr << "state 1:" << phrase1 << endl;
			cerr << "state 2:" << phrase2 << endl;
			cerr << "prefix 1:" << prefix1 << endl;
			cerr << "prefix 2:" << prefix2 << endl;
			cerr << "separator:" << suffix << endl;
			throw 0;
		}
		
		vector<symbol_> word1 = prefix1;
		word1.insert( word1.end(), suffix.begin(), suffix.end() );
		vector<symbol_> word2 = prefix2;
		word2.insert( word2.end(), suffix.begin(), suffix.end() );
		
		cout << "prefix1: " << prefix1 << " word1: " << word1 << membership_query(word1) << A_->membership_query(word1) << dfa_hp->membership_query(word1) << endl;
		cout << "prefix2: " << prefix2 << " word2: " << word2 << membership_query(word2) << A_->membership_query(word2) << dfa_hp->membership_query(word2) << endl;
		
		if(membership_query(word1) == membership_query(word2)){
			if(dfa_hp->membership_query(word1) != membership_query(word1)){
				witness_result = word1;
			}
			else{
				witness_result = word2;
			}
			cout << "\tcounterexample: " << witness_result << endl;
			return false;
		}
		else{
			if(A_->membership_query(word1) != membership_query(word1)){
				add_word(prefix1);
			}
			else{
				add_word(prefix2);
			}
		}
	}
	return true;
}