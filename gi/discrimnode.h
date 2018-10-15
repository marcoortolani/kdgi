/*
 * discrimnode.h
 *
 *  Created on: 10 set 2018
 *      Author: marco
 */

#ifndef DISCRIMNODE_H_
#define DISCRIMNODE_H_

#include <vector>
#include <list>
#include <string>
#include <set>
#include <iostream>

#define symbol_ string

using namespace std;

class DiscrimNode {
private:
	vector<symbol_> phrase_;
	DiscrimNode* child_0_;
	DiscrimNode* child_1_;
	DiscrimNode* father_;
	bool final_;

public:
	DiscrimNode(vector<symbol_> ph, DiscrimNode* father);
	~DiscrimNode();

	void set_child(DiscrimNode* child, bool what_child);
	DiscrimNode* get_child(bool what_child);
	void set_father(DiscrimNode* father);
	DiscrimNode* get_father();

	bool is_leaf();
	bool is_final();
	bool is_block_root();
	void set_final();
	bool is_accepting();
	bool is_accepting(vector<symbol_> suffix, bool& truth);
	bool is_accepting(vector<symbol_> prefix, vector<symbol_> suffix, bool& truth);

	void set_phrase(vector <symbol_> ph);
	vector<symbol_> get_phrase();

	DiscrimNode* get_block_root();

	DiscrimNode* get_lowest_common_discriminator(DiscrimNode* other_node);

	vector<DiscrimNode*> get_all_leaves();

	void destroy_all_but_leaves();
};

#endif /* DISCRIMNODE_H_ */
