/*
 * discrimnode.cc
 *
 *  Created on: 10 set 2018
 *      Author: marco
 */

#include "discrimnode.h"

DiscrimNode :: DiscrimNode(vector<symbol_> ph, DiscrimNode* father){
	phrase_ = ph;
	father_ = father;
	final_ = false;
	child_0_ = NULL;
	child_1_ = NULL;
};

DiscrimNode :: ~DiscrimNode(){
	if(child_0_ != NULL){
		delete child_0_;
	}
	if(child_1_ != NULL){
		delete child_1_;
	}
}

void DiscrimNode :: set_child(DiscrimNode* child, bool what_child){
	if(what_child){
		child_1_ = child;
	}
	else{
		child_0_ = child;
	}
};

DiscrimNode* DiscrimNode :: get_child(bool what_child){
	if(what_child){
			return child_1_;
		}
		else{
			return child_0_;
		}
};

void DiscrimNode :: set_father(DiscrimNode* father){
	father_ = father;
};

DiscrimNode* DiscrimNode :: get_father(){
	return father_;
};

bool DiscrimNode :: is_leaf(){
	return child_0_ == NULL && child_1_ == NULL;
};

bool DiscrimNode :: is_final(){
	return final_;
};

bool DiscrimNode :: is_block_root(){
	return !final_ && father_->final_;
}

void DiscrimNode :: set_final(){
	final_ = true;
};

bool DiscrimNode :: is_accepting(){
	DiscrimNode* father = get_father();
	if(father == NULL){
		cerr << "Error in DiscrimNode :: is accepting" << endl;
		throw 0;
	}

	DiscrimNode* grand = father->get_father();
	if(grand != NULL){
		return father->is_accepting();
	}

	if(!father->get_phrase().empty()){
		cerr << "Error in DiscrimNode :: is accepting" << endl;
		cerr << "first ancestor has non empty phrase" << endl;
		throw 0;
	}

	if(this == father->get_child(0)){
		return false;
	}

	if(this == father->get_child(1)){
		return true;
	}

	cerr << "Error in DiscrimNode :: is accepting" << endl;
	cerr << "father and son linked uncorrectly" << endl;
	throw 0;
}

bool DiscrimNode :: is_accepting(vector<symbol_> phrase, bool& truth){
	if(!is_leaf()){
		cerr << "Error in DiscrimNode :: is_accepting (vector<symbol_>, bool&)" << endl;
		cerr << "the node is not a leaf" << endl;
		throw 0;
	}

	vector<symbol_> prefix = get_phrase();
	DiscrimNode* disc = this;
	while(disc->get_father() != NULL){
		DiscrimNode* father = disc->get_father();
		vector<symbol_> other_phrase = prefix;
		vector<symbol_> suffix = father->get_phrase();
		other_phrase.insert(other_phrase.end(), suffix.begin(), suffix.end());
		if(other_phrase == phrase){
			if(disc == father->get_child(0)){
				truth = false;
			}
			else if(disc == father->get_child(1)){
				truth = true;
			}
			else{
				cerr << "Error in DiscrimNode :: is_accepting (vector<symbol_>, bool&)" << endl;
				throw 0;
			}
			return true;
		}
		disc = father;
	}

	return false;
}

bool DiscrimNode :: is_accepting(vector <symbol_> prefix, vector<symbol_> phrase, bool& truth){
	DiscrimNode* disc = this;
	DiscrimNode* father = get_father();
	while(father != NULL){
		vector<symbol_> other_phrase = prefix;
		vector<symbol_> suffix = father->get_phrase();
		other_phrase.insert(other_phrase.end(), suffix.begin(), suffix.end());
		if(other_phrase == phrase){
			if(disc == father->get_child(0)){
				truth = false;
			}
			else if(disc == father->get_child(1)){
				truth = true;
			}
			else{
				cerr << "Error in DiscrimNode :: is_accepting (vector<symbol_>, vector<symbol_>, bool&)" << endl;
				throw 0;
			}
			return true;
		}
		disc = father;
		father = disc->get_father();
	}

	return false;
}

void DiscrimNode :: set_phrase(vector <symbol_> ph){
	phrase_ = ph;
};

vector<symbol_> DiscrimNode :: get_phrase(){
	return phrase_;
};

DiscrimNode* DiscrimNode :: get_block_root(){

	if(final_){
		cerr << "Error in DiscrimNode :: get_block_root" << endl;
		cerr << "a final node is not part of any block" << endl;
		throw 0;
	}

	DiscrimNode* d = this;
	for(;!d->get_father() == NULL && !d->get_father()->is_final(); d = d->get_father());

	return d;
}

DiscrimNode* DiscrimNode :: get_lowest_common_discriminator(DiscrimNode* other_node){

	vector<symbol_> test0 = {"a", "b", "a", "a", "b"};
	vector<symbol_> test1 = {"a", "a", "b", "a", "a", "b"};

	if(other_node == NULL){
		cerr << "Error in DiscrimNode :: get_lowes_common_discriminator" << endl;
		cerr << "other_node can't be NULL" << endl;
		throw 0;
	}

	if(this == other_node){
		return this;
	}

	DiscrimNode* current_ancestor = this;
	DiscrimNode* other_current_ancestor = other_node;
	set<DiscrimNode*> this_ancestors;
	set<DiscrimNode*> other_ancestors;
	while(current_ancestor != NULL && other_current_ancestor != NULL){
		if(current_ancestor == other_current_ancestor){
			return current_ancestor;
		}
		if(other_ancestors.find(current_ancestor) != other_ancestors.end()){
			return current_ancestor;
		}

		if(this_ancestors.find(other_current_ancestor) != this_ancestors.end()){
			return other_current_ancestor;
		}

		this_ancestors.insert(current_ancestor);
		other_ancestors.insert(other_current_ancestor);

		current_ancestor = current_ancestor->get_father();
		other_current_ancestor = other_current_ancestor->get_father();
	}

	while(current_ancestor != NULL){
		if(other_ancestors.find(current_ancestor) != other_ancestors.end()){
			return current_ancestor;
		}

		current_ancestor = current_ancestor->get_father();
	}

	while(other_current_ancestor != NULL){
		if(this_ancestors.find(other_current_ancestor) != this_ancestors.end()){
			return other_current_ancestor;
		}

		other_current_ancestor = other_current_ancestor->get_father();
	}

	cerr << "Error in DiscrimNode :: get_lowest_common_discriminator" << endl;
	cerr << "could not find a common ancestor between ";
	if(this->is_leaf()){
		cerr << " leaf ";
	}
	else{
		cerr << " node ";
	}
	for(symbol_ s : this->get_phrase()){
		cerr << s;
	}
	if(other_node->is_leaf()){
		cerr << " and leaf ";
	}
	else{
		cerr << " and node ";
	}
	for(symbol_ s : other_node->get_phrase()){
		cerr << s;
	}
	cerr << endl;
	throw 0;
}

vector<DiscrimNode*> DiscrimNode :: get_all_leaves(){
	vector<DiscrimNode*> result;
	list<DiscrimNode*> next_nodes = {this};

	while(!next_nodes.empty()){
		DiscrimNode* node = next_nodes.back();
		next_nodes.pop_back();

		bool leaf = true;
		for(int i=0; i < 2; ++i){
			DiscrimNode* child = node->get_child(i);
			if(child != NULL){
				leaf = false;
				next_nodes.push_front(child);
			}
		}

		if(leaf){
			result.push_back(node);
		}
	}

	return result;
}

void DiscrimNode :: destroy_all_but_leaves(){
	bool leaf = true;
	if(child_0_ != NULL){
		child_0_->destroy_all_but_leaves();
		leaf = false;
	}
	if(child_1_ != NULL){
		child_1_->destroy_all_but_leaves();
		leaf = false;
	}

	if(!leaf){
		/*cout << "distruggo nodo ";
		for(symbol_ s : phrase_){
			cout << s ;
		}
		cout << endl;*/
		child_0_ = NULL;
		child_1_ = NULL;
		delete this;
	}
}
