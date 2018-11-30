/*
 * tttdfa.cc
 *
 *  Created on: 07 set 2018
 *      Author: marco
 */

#include "tttdfa.h"

bool DEBUG = false;

bool TTTDfa::membership_query(vector<symbol_> phrase) const {
	DfaState* state = span_tree_;
	
	for(symbol_ sym : phrase){
		state = next_span_state(state, sym, false);
	}
	return state->is_accepting();
}

list<DfaState>::iterator TTTDfa::begin(){
	return states_.begin();
}

list<DfaState>::iterator TTTDfa::end(){
	return states_.end();
}

TTTDfa :: TTTDfa(vector<symbol_> alph, vector<bool> first_membership_queries){
	if(alph.size() != first_membership_queries.size() - 1){
		cerr << "Error in initialization of TTTDfa" << endl;
		throw 0;
	}

	Dfa::set_alphabet(alph);
	Dfa::set_num_state(0);

	span_tree_ = new DfaState(first_membership_queries.back(), vector<symbol_>(), num_states_++);
	discrim_tree_ = new DiscrimNode(vector<symbol_>(), NULL);
	discrim_tree_->set_child(new DiscrimNode(vector<symbol_>(), discrim_tree_), first_membership_queries.back());
	discrim_tree_->set_final();

	span_disc_link_[vector<symbol_>()] = make_pair(span_tree_, discrim_tree_->get_child(first_membership_queries.back()));

	DfaState* other_node = NULL;

	for(int i = 0; i < alph.size(); ++i){
		if(first_membership_queries[i] == first_membership_queries.back()){
			set_non_tree_transition(span_tree_, alph[i], discrim_tree_->get_child(first_membership_queries[i]));
		}
		else{
			if(other_node == NULL){
				other_node = set_final_transition(span_tree_, alph[i], discrim_tree_, !first_membership_queries.back());
			}
			else{
				set_non_tree_transition(span_tree_, alph[i], discrim_tree_->get_child(first_membership_queries[i]));
			}
		}
	}
}

TTTDfa :: ~TTTDfa(){
	for(auto pair : span_disc_link_){
		DfaState* s = pair.second.first;
		delete s;
	}

	delete discrim_tree_;
}

DfaState* TTTDfa :: set_final_transition(DfaState* start_state, symbol_ transition, DiscrimNode* father, bool child){
	if(father == NULL){
		cerr << "Error in TTTDfa :: set_deterministic_transition" << endl;
		cerr << "passed a NULL pointer as father node" << endl;
		throw 0;
	}
	if(father->get_child(child) != NULL){
		cerr << "Error in TTTDfa :: set_deterministic_transition" << endl;
		cerr << "the " << child << " child of the node " << father->get_phrase() << "is not a new state" << endl;
		cerr << "(i.e. it's not a NULL pointer)" << endl;
		throw 0;
	}

	vector <symbol_> new_phrase = start_state->get_charact_phrase();
	new_phrase.push_back(transition);

	bool accepting;
	if(father->get_father() != NULL){
		accepting = father->is_accepting();
	}
	else{
		accepting = child;
	}
	DfaState* arrive_state = new DfaState(accepting, new_phrase, num_states_++);

	start_state->set_transition(transition, arrive_state);
	non_tree_transitions_.erase_non_tree_transition(start_state->get_charact_phrase(), transition);

	father->set_child(new DiscrimNode(new_phrase, father), child);

	span_disc_link_[new_phrase] = std::make_pair(arrive_state, father->get_child(child));
	return arrive_state;
}

void TTTDfa :: set_non_tree_transition(DfaState* state, symbol_ transition, DiscrimNode* disc_node){
	state->set_transition(transition, &non_tree_transitions_);
	non_tree_transitions_.set_non_tree_transition(state->get_charact_phrase(), transition, disc_node);
}

pair<DfaState*, DiscrimNode*> TTTDfa :: get_state_leaf_link(vector<symbol_> phrase) const{
	auto it_pair = span_disc_link_.find(phrase);
	if(it_pair == span_disc_link_.end()){
		cerr << "Error in TTTDfa :: get_state_leaf_link" << endl;
		cerr << "could not find the pair of state,leaf related to the phrase:" << phrase << endl;
		throw 0;
	}

	return it_pair->second;
}

DfaState* TTTDfa :: next_span_state(DfaState* start, symbol_ transition, bool strict) const{
	DfaState* arrive_state = start->next(transition, false);
	if(arrive_state == NULL){
		cerr << "Error in TTTDfa :: next_span_state" << endl;
		cerr << "State " << start->get_charact_phrase() << " has no transition for symbol " << transition << endl;
		throw 0;
	}

	if(arrive_state == &non_tree_transitions_){
		DiscrimNode* arrive_node = non_tree_transitions_.get_non_tree_arrive_node(start->get_charact_phrase(), transition);
		bool leaf = arrive_node->is_leaf();
		if(strict && !leaf){
			cerr << "Error in TTTDfa :: next_span_state" << endl;
			cerr << "the Dfa is still non deterministic, cannot reach any state for the phrase " << start->get_charact_phrase() << " " << transition << endl;
			throw 0;
		}

		if(!leaf){
			arrive_node = arrive_node->get_all_leaves().front();
		}

		arrive_state = get_state_leaf_link(arrive_node->get_phrase()).first;
	}

	return arrive_state;
}

DiscrimNode* TTTDfa :: next_disc_node(DiscrimNode* start, symbol_ transition) const{
	if(start == NULL){
		cerr << "Error in TTTDfa :: next_disc_node" << endl;
		cerr << "the node passed is NULL" << endl;
		throw 0;
	}

	if(!start->is_leaf()){
		cerr << "Error in TTTDfa :: next_disc_node" << endl;
		cerr << "a transition must start from a leaf node" << endl;
		throw 0;
	}

	DfaState* state = get_state_leaf_link(start->get_phrase()).first;

	state = state->next(transition);
	if(state == &non_tree_transitions_){
		return non_tree_transitions_.get_non_tree_arrive_node(start->get_phrase(), transition);
	}
	else{
		return get_state_leaf_link(state->get_charact_phrase()).second;
	}
}

bool TTTDfa :: test_closure(DfaState* start, list<symbol_> discriminator, bool expected_value, bool strict){
	for(symbol_ sym : discriminator){
		start = next_span_state(start, sym, strict);
	}
	return expected_value == start->is_accepting();
}

void TTTDfa :: split_state(DfaState* start_state, symbol_ transition, vector<symbol_> discriminator, bool state_position, bool strict){
	vector<symbol_> old_state_phrase = next_span_state(start_state, transition, strict)->get_charact_phrase();
	vector<symbol_> new_state_phrase = start_state->get_charact_phrase();
	new_state_phrase.push_back(transition);

	start_state->set_transition(transition, new DfaState(next_span_state(start_state,transition, strict)->is_accepting(), new_state_phrase, num_states_++));
	non_tree_transitions_.erase_non_tree_transition(start_state->get_charact_phrase(), transition);

	DiscrimNode* father = get_state_leaf_link(old_state_phrase).second;
	DiscrimNode* ch_old = new DiscrimNode(old_state_phrase, father);
	DiscrimNode* ch_new = new DiscrimNode(new_state_phrase, father);

	father->set_phrase(discriminator);
	//Inserire codice per il controllo sulla finalizzazione del nodo padre
	father->set_child(ch_old, state_position);
	father->set_child(ch_new, !state_position);

	span_disc_link_[old_state_phrase].second = ch_old;
	span_disc_link_[new_state_phrase].first = start_state->next(transition);
	span_disc_link_[new_state_phrase].second = ch_new;
}

bool TTTDfa :: handle_counterexample(vector <symbol_> cntr_ex, bool cntr_truth){
	DfaState* state = span_tree_;
	for(symbol_ sym : cntr_ex){
		state = next_span_state(state, sym, false);
	}

	if(cntr_truth == state->is_accepting())
		return false;

	state = span_tree_;
	
	list<symbol_> suffix;
	suffix.insert(suffix.end(), cntr_ex.begin(), cntr_ex.end());
	while(!suffix.empty()){
		symbol_ transition = suffix.front();

		DfaState* next_state = state->next(transition);
		if(next_state == &non_tree_transitions_ && test_closure(state, suffix, !cntr_truth, false)){
			suffix.pop_front();

			vector<symbol_> suff_vec;
			suff_vec.insert(suff_vec.end(), suffix.begin(), suffix.end());
			split_state(state, transition, suff_vec, !cntr_truth, false);

			if(DEBUG)
				cout << "trovata suddivisione del controesempio " << state->get_charact_phrase() << " : " << transition << " : " << suff_vec << endl;

			return true;
		}

		if(next_state == NULL){
			cerr << "Error in TTTDfa :: handle_counterexample:" << endl;
			cerr << "Found a NULL transition" << endl;
			throw 0;
		}
		
		if(next_state == &non_tree_transitions_){
			cerr << "Error in TTTDfa :: handle_counterexample:" << endl;
			cerr << "Found a non tree transition not causing a subdvivision of the counterexample" << endl;
			throw 0;
		}

		suffix.pop_front();
		state = next_state;
	}

	cerr << "Error in TTTDfa :: handle_counterexample:" << endl;
	cerr <<"A subdivision of the counterexample (Rivest Schapire) couldn't be found" << endl;
	throw 0;
}

vector<std::tuple<vector<symbol_>, symbol_, bool>> TTTDfa :: get_transitions_to_sift(bool hard_sift){
	vector<std::tuple<vector<symbol_>, symbol_, bool>> res;
	vector <symbol_> sorted_alphabet = sort_alphabet();
	for(auto pair : span_disc_link_){
		DfaState* state = pair.second.first;
		for(symbol_ sym : sorted_alphabet){
			DfaState* next_state = state->next(sym, false);

			if(next_state == NULL){
				res.push_back(std::make_tuple(state->get_charact_phrase(), sym, hard_sift));
			}
			else if(next_state == &non_tree_transitions_){
				DiscrimNode* disc = non_tree_transitions_.get_non_tree_arrive_node(state->get_charact_phrase(), sym);
				if(!disc->is_leaf()){
					if(hard_sift || (!disc->is_final() && !disc->is_block_root())){
						res.push_back(std::make_tuple(state->get_charact_phrase(), sym, true));
					}
					else if(disc->is_final()){
						res.push_back(std::make_tuple(state->get_charact_phrase(), sym, false));
					}
				}
			}
		}
	}

	return res;
}

void TTTDfa :: init_sifting(vector<symbol_> prefix, symbol_ transition, vector<symbol_>& suffix, bool hard_sift){
	DfaState* state = get_state_leaf_link(prefix).first;
	state = state->next(transition, false);
	if(state != NULL){
		if(state != &non_tree_transitions_){
			cerr << "Error in TTTDfa :: init_sifting " << endl;
			cerr << "cannot sift the final transition" << prefix << transition << endl;
			throw 0;
		}
		else{
			DiscrimNode* disc = non_tree_transitions_.get_non_tree_arrive_node(prefix, transition);
			if(!hard_sift && !disc->is_final()){
				cerr << "Error in TTTDfa :: init_sifting " << endl;
				cerr << "cannot soft sift the transition " << prefix << transition << " anymore" << endl;
				cerr << "to sift more use hard sifting" << endl;
				throw 0;
			}
			else{
				utility_node_ = disc;
			}
		}
	}
	else{
		utility_node_ = discrim_tree_;
	}

	suffix = utility_node_->get_phrase();
	return;
}

bool TTTDfa :: sift_step(vector<symbol_>& suffix, bool child, bool hard_sift){
	if(utility_node_ == NULL){
		cerr << "Error in TTTDfa :: sift_step " << endl;
		cerr << "utility_node_ was not set" << endl;
		throw 0;
	}

	if(!utility_node_->is_final() && !hard_sift){
		cerr << "Error in TTTDfa :: sift_step" << endl;
		cerr << "the sifting is continuing inside a block despite it being soft" << endl;
		throw 0;
	}

	DiscrimNode* next = utility_node_->get_child(child);
	if(next != NULL){
		if(next->is_leaf()){
			return false;
		}
		else if(hard_sift || next->is_final()){
			utility_node_ = next;
			suffix = utility_node_->get_phrase();
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

void TTTDfa :: close_transition(vector<symbol_> state_phrase, symbol_ transition, bool child){
	DfaState* state = get_state_leaf_link(state_phrase).first;
	if(utility_node_ == NULL){
		cerr << "Error in TTTDfa :: close_transition" << endl;
		cerr << "no arrive node selected" << endl;
		throw 0;
	}
	else {
		if(utility_node_->get_child(child) != NULL){
			set_non_tree_transition(state, transition, utility_node_->get_child(child));
		}
		else{
			set_final_transition(state, transition, utility_node_, child);
		}

		utility_node_ = NULL;
	}
}

bool is_prefix(vector<symbol_> prefix, vector<symbol_> word){
	if(word.size() < prefix.size()){
		return false;
	}

	for(int i = 0; i < prefix.size(); ++i){
		if(prefix[i] != word[i]){
			return false;
		}
	}

	return true;
}

bool TTTDfa :: know_phrase_for_sure(vector<symbol_> phrase, bool& truth){
	//This cause the error for some random languages.
	//The map is badly generated so it is not used for now.
	/*auto it = lost_queries_.find(phrase);
	if(it != lost_queries_.end()){
		truth = it->second;
		return true;
	}
	
	return false;*/

	DfaState* state = span_tree_;
	DfaState* next = NULL;
	for(int i = 0; i < phrase.size(); ++i){
		DiscrimNode* leaf = get_state_leaf_link(state->get_charact_phrase()).second;
		if(leaf->is_accepting(phrase, truth))
			return true;

		next = state->next(phrase[i], false);
		if(next == NULL){
			return false;
		}
		else if(next == &non_tree_transitions_){
			vector<symbol_> prefix = state->get_charact_phrase();
			DiscrimNode* disc = non_tree_transitions_.get_non_tree_arrive_node(prefix, phrase[i]);
			prefix.push_back(phrase[i]);

			if(disc->is_accepting(prefix, phrase, truth)){
				return true;
			}

			return false;
		}
		else{
			state = next;
		}
	}

	truth = state->is_accepting();
	return true;
}

bool TTTDfa :: try_single_finalization(vector<vector<symbol_>>& prefixes_to_verify, vector<vector<symbol_>>& prefixes0, vector<vector<symbol_>>& prefixes1, vector<symbol_>& suffix){
	vector<vector<DiscrimNode*>> blocks = group_leaves_into_blocks();
	vector<symbol_> sorted_alphabet = sort_alphabet();

	vector<vector<symbol_>> temp_prefixes;

	int length = 0;
	for(auto block : blocks){
		if(block.size() > 1){
			for(symbol_ sym : sorted_alphabet){
				DiscrimNode* node = block[0];
				DiscrimNode* transition_root = next_disc_node(node, sym)->get_block_root();

				for(int i = 1; i < block.size(); ++i){
					node = block[i];
					DiscrimNode* other_transition_root = next_disc_node(node, sym)->get_block_root();
					if(transition_root != other_transition_root){
						temp_prefixes.clear();
						for(DiscrimNode* leaf : block){
							temp_prefixes.push_back(leaf->get_phrase());
						}

						utility_node_ = node->get_block_root();
						DiscrimNode* lcd = transition_root->get_lowest_common_discriminator(other_transition_root);
						vector <symbol_> tmp = lcd->get_phrase();

						if(length == 0 || length > tmp.size() + 1){
							suffix.clear();
							suffix.push_back(sym);
							suffix.insert(suffix.end(), tmp.begin(), tmp.end());
							length = suffix.size();
						}
					}
				}
			}
		}
		if(length > 0){
			break;
		}
	}

	if(length > 0){
		prefixes_to_verify.clear();
		prefixes0.clear();
		prefixes1.clear();
		for(vector<symbol_> prefix : temp_prefixes){
			vector<symbol_> phrase = prefix;
			phrase.insert(phrase.end(), suffix.begin(), suffix.end());
			bool x;
			if(know_phrase_for_sure(phrase, x)){
				if(x){
					prefixes1.push_back(prefix);
				}
				else{
					prefixes0.push_back(prefix);
				}
			}
			else{
				prefixes_to_verify.push_back(prefix);
			}
		}

		if( (prefixes_to_verify.empty() && prefixes0.empty()) || (prefixes_to_verify.empty() && prefixes1.empty()) ){
			cerr << "Error in TTTDfa :: try_single_finalization" << endl;
			throw 0;
		}

		return true;
	}

	utility_node_ = NULL;
	return false;
}

DiscrimNode* gen_block_tree(set<DiscrimNode*> nodes, DiscrimNode* old_root){
	if(old_root == NULL || old_root->is_leaf()){
		return old_root;
	}

	if(old_root->get_child(0) != NULL && old_root->get_child(1) != NULL){
		if(nodes.find(old_root->get_child(0)) != nodes.end() && nodes.find(old_root->get_child(1)) != nodes.end()){
			DiscrimNode* copy = new DiscrimNode(old_root->get_phrase(), NULL);
			copy->set_child(gen_block_tree(nodes, old_root->get_child(0)), 0);
			copy->get_child(0)->set_father(copy);
			copy->set_child(gen_block_tree(nodes, old_root->get_child(1)), 1);
			copy->get_child(1)->set_father(copy);

			return copy;
		}
	}

	if(old_root->get_child(0) != NULL && nodes.find(old_root->get_child(0)) != nodes.end()){
		return gen_block_tree(nodes, old_root->get_child(0));
	}

	if(old_root->get_child(1) != NULL && nodes.find(old_root->get_child(1)) != nodes.end()){
		return gen_block_tree(nodes, old_root->get_child(1));
	}

	cerr << "Error in TTTDfa :: gen_block_tree" << endl;
	for(DiscrimNode* node : nodes){
		cerr << node->get_phrase() << " : " << node->is_accepting() << endl;
	}
	throw 0;
}

vector<DiscrimNode*> get_all_block_leaves(DiscrimNode* root){
	if(root == NULL){
		cerr << "Error in TTTDfa :: get_all_block_leaves" << endl;
		cerr << "root is NULL" << endl;
		throw 0;
	}

	if(!root->is_block_root()){
		cerr << "Error in TTTDfa :: get_all_block_leaves" << endl;
		throw 0;
	}

	return root->get_all_leaves();
}

void TTTDfa :: mark_nodes(vector<pair<vector<symbol_>, bool>> leaf_queries, set<DiscrimNode*>& mark_0, set<DiscrimNode*>& mark_1){
	vector<list<DiscrimNode*>> nodes = {list<DiscrimNode*>(), list<DiscrimNode*>()};

	for(auto pair : leaf_queries){
		DiscrimNode* leaf = get_state_leaf_link(pair.first).second;
		if(pair.second){
			mark_1.insert(leaf);
			nodes[1].push_front(leaf->get_father());
		}
		else{
			mark_0.insert(leaf);
			nodes[0].push_front(leaf->get_father());
		}
	}

	for(int i = 0; i < 2; ++i){
		set<DiscrimNode*>* mark;
		if(i){
			mark = &mark_1;
		}
		else{
			mark = &mark_0;
		}

		while(!nodes[i].empty()){
			DiscrimNode* node = nodes[i].back();
			nodes[i].pop_back();

			mark->insert(node);

			if(node != utility_node_){
				nodes[i].push_front(node->get_father());
			}
		}
	}
}

void TTTDfa :: update_lost_queries(vector<pair<vector<symbol_>, bool>> leaf_queries, set<DiscrimNode*> mark_0, set<DiscrimNode*> mark_1){
	vector<symbol_> old_suffix = utility_node_->get_phrase();
	for(auto leaf_query : leaf_queries){
		vector<symbol_> phrase = leaf_query.first;
		DiscrimNode* disc = get_state_leaf_link(phrase).second;
		phrase.insert(phrase.end(), old_suffix.begin(), old_suffix.end());

		bool truth;
		if(disc->is_accepting(phrase, truth))
			lost_queries_[phrase] = truth;
		else{
			cerr << "Error in TTTDfa :: update_lost_queries" << endl;
			cerr << "Couldn't find the phrase " << phrase << " as ancestor of " << disc->get_phrase() << " (prefix-suffix)" << endl;
			throw 0;
		}

		if(mark_0.find(disc) != mark_0.end()){
			truth = false;
		}
		else if(mark_1.find(disc) != mark_1.end()){
			truth = true;
		}
		else{
			cerr << "Error in TTTDfa :: update_lost_queries" << endl;
			cerr << "Couldn't find the node " << phrase << " as in mark_0 or mark_1" << endl;
			throw 0;
		}

		vector<symbol_> prefix = disc->get_phrase();

		bool test = true;
		while(test){
			if(disc == utility_node_)
				test = false;

			DiscrimNode* father = disc->get_father();
			set<DiscrimNode*>* mark;
			if(truth){
				mark = &mark_0;
			}
			else{
				mark = &mark_1;
			}

			if(mark->find(father) == mark->end()){
				phrase = prefix;
				vector<symbol_> suffix = father->get_phrase();
				phrase.insert(phrase.end(), suffix.begin(), suffix.end());

				if(disc == father->get_child(0)){
					lost_queries_[phrase] = false;
				}
				else if(disc == father->get_child(1)){
					lost_queries_[phrase] = true;
				}
				else{
					cerr << "ERRORE QUI" << endl;
					throw 0;
				}
			}
			disc = father;
		}
	}
}

void TTTDfa :: split_block(vector<pair<vector<symbol_>, bool>> leaf_queries, vector<symbol_> suffix){
	if(utility_node_ == NULL || !utility_node_->is_block_root()){
		cerr << "Error in TTTDfa :: split_block" << endl;
		throw 0;
	}

	set<DiscrimNode*> mark_0;
	set<DiscrimNode*> mark_1;

	mark_nodes(leaf_queries, mark_0, mark_1);

	if(mark_0.empty() || mark_1.empty()){
		cerr << "Error in TTTDfa :: split_block" << endl;
		cerr << "Block cannot be split if there aren't 2 states with different outcome for suffix: " << suffix << endl;
		//print_ttt_dot("", "debug.dot");
		throw 0;
	}

	if(utility_node_->get_phrase() != suffix){
		update_lost_queries(leaf_queries, mark_0, mark_1);

		DiscrimNode* tree_0 = gen_block_tree(mark_0, utility_node_);
		tree_0->set_father(utility_node_);
		DiscrimNode* tree_1 = gen_block_tree(mark_1, utility_node_);
		tree_1->set_father(utility_node_);

		utility_node_->get_child(false)->destroy_all_but_leaves();
		utility_node_->get_child(true)->destroy_all_but_leaves();
		utility_node_->set_child(tree_0, false);
		utility_node_->set_child(tree_1, true);
		utility_node_->set_phrase(suffix);
	}

	utility_node_->set_final();
	utility_node_ = NULL;
}

vector<vector<DiscrimNode*>> TTTDfa :: group_leaves_into_blocks(){
	vector<vector<DiscrimNode*>> blocks;

	list<DiscrimNode*> next_nodes = {discrim_tree_};

	do{
		DiscrimNode* disc = next_nodes.back();
		next_nodes.pop_back();

		if(!disc->is_final() && !disc->is_block_root()){
			cerr << "Error in TTTDfa :: group_leaves_into_blocks" << endl;
			cerr << "for node " << disc->get_phrase();
			throw 0;
		}

		if(disc->is_block_root()){
			blocks.push_back(get_all_block_leaves(disc));
		}
		else{
			if(disc->get_child(0) != NULL){
				next_nodes.push_front(disc->get_child(0));
			}
			if(disc->get_child(1) != NULL){
				next_nodes.push_front(disc->get_child(1));
			}
		}
	}while(!next_nodes.empty());

	return blocks;
}

void TTTDfa :: make_hypotesis(){
	states_.clear();

	if(span_tree_ == NULL){
		return;
	}

	vector<symbol_> sorted_alphabet = sort_alphabet();
	set<DfaState*> states_found;
	list<DfaState*> next_states;
	list<vector<symbol_>> next_phrases;
	next_states.push_front(span_tree_);
	next_phrases.push_front(vector<symbol_>());

	map<vector<symbol_>, vector<symbol_>> translator;
	translator[vector<symbol_>()] = vector<symbol_>();
	int i = 0;
	while(!next_states.empty()){
		DfaState* state = next_states.back();
		vector<symbol_> phrase = next_phrases.back();
		next_states.pop_back();
		next_phrases.pop_back();

		if(states_found.find(state) == states_found.end()){
			states_found.insert(state);
			states_.emplace_back(state->is_accepting(), phrase, i++);
			translator[state->get_charact_phrase()] = phrase;
			for(symbol_ sym : sorted_alphabet){
				DfaState* next_state = TTTDfa :: next_span_state(state, sym, false);

				if(states_found.find(next_state) == states_found.end()){
					next_states.push_front(next_state);
					vector <symbol_> next_phrase = phrase;
					next_phrase.push_back(sym);
					next_phrases.push_front(next_phrase);
				}
			}
		}
	}

	for(auto generic_state_it = this->begin(); generic_state_it != this->end(); ++generic_state_it){
		DfaState* tttstate = span_tree_;
		for(symbol_ sym : generic_state_it->get_charact_phrase()){
			tttstate = TTTDfa :: next_span_state(tttstate, sym);
		}

		for(symbol_ s : sorted_alphabet){
			DfaState* next_tttstate = TTTDfa :: next_span_state(tttstate, s, false);
			
			bool found = false;
			for(auto state_it = this->begin(); !found && state_it != this->end(); ++state_it){
				if(state_it->get_charact_phrase() == translator[next_tttstate->get_charact_phrase()]){
					found = true;
					generic_state_it->set_transition(s, &(*state_it));
					state_it->set_incoming_transiction(std :: make_pair(&(*generic_state_it), s));
				}
			}
			if(!found){
				cerr << "Error in TTTDfa :: make_hypotesis" << endl;
				cerr << "couldn't create the dfa" << endl;
				throw 0;
			}
		}
	}
}

void TTTDfa :: print_span() const {
	for(auto pair : span_disc_link_){
		DfaState* state = pair.second.first;
		cout << "stato :" << state->get_charact_phrase() << " accettante: " << state->is_accepting() << endl;

		vector<symbol_> sorted_alphabet = sort_alphabet();
		for(symbol_ s : sorted_alphabet){
			cout << "\tsimbolo :" << s << endl;
			DfaState* next = state->next(s, false);
			if(next == NULL){
				cout << "\ttransizione non trovata" << endl;
			}
			else if(next == &non_tree_transitions_){
				DiscrimNode* d = non_tree_transitions_.get_non_tree_arrive_node(state->get_charact_phrase(), s);
				if(d->is_leaf()){
					cout << "\ttransizione non-definitiva verso foglia:" << d->get_phrase() << endl;
				}
				else{
					cout << "\ttransizione non-deterministica verso nodo:" << d->get_phrase();
					if(!d->is_block_root()){
						cout << " NON BLOCK ROOT";
					}
					cout << endl;
				}
			}
			else{
				cout << "\ttransizione verso stato:" << next->get_charact_phrase() << endl;
			}
		}
	}
}

void TTTDfa :: print_disc() const {
	list<DiscrimNode*> l = {discrim_tree_};

	while(!l.empty()){
		DiscrimNode* n = l.back();
		l.pop_back();

		cout << n->get_phrase() << endl;
		if(n->is_leaf()){
			cout << "\tfoglia" << endl;
		}
		else{
			cout << "\tfinale:" << n->is_final() << endl;
		}

		if(n->get_child(0) != NULL){
			l.push_front(n->get_child(0));
			cout << "\t0: " << n->get_child(0)->get_phrase() << endl;
		}
		if(n->get_child(1) != NULL){
			l.push_front(n->get_child(1));
			cout << "\t1: " << n->get_child(1)->get_phrase() << endl;
		}
	}
}

void TTTDfa :: print() const{
	cout << "span tree:" << endl;
	print_span();
	cout << "discrimination tree" << endl;
	print_disc();
	cout << endl;
}

bool TTTDfa :: is_deterministic() const{
	for(auto pair_it = span_disc_link_.begin(); pair_it != span_disc_link_.end(); ++pair_it){
		DfaState* start_state = pair_it->second.first;
		vector<symbol_> sorted_alphabet = sort_alphabet();
		for(symbol_ sym : sorted_alphabet){
			DfaState* arrive_state = start_state->next(sym, false);
			if(arrive_state == NULL){
				return false;
			}
			else if(arrive_state == &non_tree_transitions_){
				DiscrimNode* arrive_node = non_tree_transitions_.get_non_tree_arrive_node(start_state->get_charact_phrase(), sym);
				if(!arrive_node->is_leaf()){
					return false;
				}
			}
		}
	}

	return true;
}

void TTTDfa :: print_ttt_dot(string title, string path){
	ofstream myfile;
	myfile.open(path);

	// Initialization of symbol_s with DOT code
	string header = "digraph "+title+" {\n";
	string start_state_ = "__start0 [label=\"\" shape=\"none\"];\n\n";

	start_state_ = start_state_ + "\nsize=\"8,5\";\n\n";


	//States
	string states = "";
	string ranks = "";
	string transitions = "";

	string shape = "";
	string style= "";
	string color= "white";

	for(auto pair : span_disc_link_)
	{
		DfaState* state = pair.second.first;
		if(state->is_accepting()){
			shape = "doublecircle";
			style = "rounded,filled";
		}
		else{
			shape = "circle";
			style = "filled";
		}

		string label = "";
		for(symbol_ sym : state->get_charact_phrase())
			label += sym;

		states = states + "s" + label + " [style=\""+style+"\", color=\"black\", fillcolor=\""+color+"\" shape=\""+shape+"\", label=\""+label+"\"];\n";

		shape = "square";
		style = "filled";
		states = states + "l" + label + " [style=\""+style+"\", color=\"black\", fillcolor=\""+color+"\" shape=\""+shape+"\", label=\""+label+"\"];\n";

		for(symbol_ sym : get_alphabet()){
			DfaState* next = state->next(sym, false);
			if(next != NULL){
				string next_label = "";
				string id = "";
				if(next != &non_tree_transitions_){
					id = "s";
					for(symbol_ sym : next->get_charact_phrase())
						next_label += sym;
				}
				else{
					DiscrimNode* disc = non_tree_transitions_.get_non_tree_arrive_node(state->get_charact_phrase(), sym);
					if(disc->is_leaf()){
						id = "l";
						for(symbol_ sym : disc->get_phrase())
							next_label += sym;
					}
					else{
						id = "n";
						next_label = disc->get_ancestors_string();
					}
				}
				transitions = transitions + "s"+ label +" -> "+ id + next_label +" [label=\""+ sym +"\"];\n";
			}
		}
	}

	list<pair<DiscrimNode*, int>> next_nodes = {std :: make_pair(discrim_tree_, 0)};
	int current_depth = 0;
	ranks += "{rank=same; ";
	while(!next_nodes.empty()){
		auto pair = next_nodes.back();
		DiscrimNode* disc = pair.first;
		if(current_depth != pair.second){
			current_depth++;
			ranks += " }\n{rank=same; ";
		}

		next_nodes.pop_back();

		string type = "";
		string label = "";
		string id = "";

		for(symbol_ sym : disc->get_phrase())
			label += sym;

		if(disc->is_leaf()){
			shape = "rectangle";
			style = "filled";
			type = "l";
			id = label;
		}
		else{
			shape = "ellipse";
			type = "n";
			if(disc->is_final()){
				style = "filled";
			}
			else{
				style = "dotted";
			}
			id = disc->get_ancestors_string();
		}
		states = states + type + id + " [style=\""+style+"\", color=\"black\", fillcolor=\""+color+"\" shape=\""+shape+"\", label=\""+label+"\"];\n";
		ranks += type + id + "; ";

		for(int b = 0; b < 2; ++b){
			DiscrimNode* child = disc->get_child(b);
			if(child != NULL){
				string child_label = "n";
				if(child->is_leaf()){
					child_label = "l";
					for(symbol_ sym : child->get_phrase())
						child_label += sym;
				}
				else
					child_label += child->get_ancestors_string();

				transitions = transitions + type + id +" -> "+ child_label +" [label=\""+std :: to_string(b)+"\"];\n";
				next_nodes.push_front(std :: make_pair(child, current_depth + 1));
			}
		}
	}
	ranks += "}\n";


	symbol_ end = "__start0 -> 0;";
	symbol_ footer ="\n}";


	// Finally, it prints overall DOT code

	myfile << header << start_state_ << states << ranks << transitions << footer;

	myfile.close();
}
