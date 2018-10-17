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
	vector<symbol_> phrase_;	/*!< A phrase that characterize the node, its meaning change if the node is a leaf or not */
	DiscrimNode* child_0_;		/*!< A pointer to the left child of the node */
	DiscrimNode* child_1_;		/*!< A pointer to the right child of the node */
	DiscrimNode* father_;		/*!< A pointer to the father of the node */
	bool final_;				/*!< Not used by Ob. Pack algorithm, indicates if a non-leaf node is final or not */

public:
	/**
	 * Constructor.
	 * @param ph		The phrase associated to this node.
	 * @param father	A pointer to the father of this node.
	 */
	DiscrimNode(vector<symbol_> ph, DiscrimNode* father);

	/**
	 * Destructor.
	 */
	~DiscrimNode();

	/**
	 * Set a child (right or left) to this node.
	 * @param child			The pointer to the new child.
	 * @param what_child	True if the child is the right one false if it is the left one.
	 */
	void set_child(DiscrimNode* child, bool what_child);

	/**
	 * Get one of the node childrens.
	 * @param what_child	True if the child is the right one false if it is the left one.
	 * @return				A pointer to the chosen child.
	 */
	DiscrimNode* get_child(bool what_child);

	/**
	 * Set the father of this node.
	 * @param father	A pointer to the father.
	 */
	void set_father(DiscrimNode* father);

	/**
	 * Return the father of this node, NULL if this node is the tree root.
	 * @return A pointer to the node's father.
	 */
	DiscrimNode* get_father();

	/**
	 * Tells if the node is a leaf.
	 * @return True if the node is a leaf, false otherwise.
	 */
	bool is_leaf();

	/**
	 * Tells if the node is final (used for TTT algorithm).
	 * @return True if the node is final, false otherwise.
	 */
	bool is_final();

	/**
	 * Tells if the node is a block root i.e. if its father (and so all its ancestors) has been finalized but the node (and all his sucessors)
	 * hasn't been yet.
	 * @return True if the node is a block root, false otherwise.
	 */
	bool is_block_root();

	/**
	 * Set the flag final to true because the node has been finalized.
	 */
	void set_final();

	/**
	 * Tells if the node is accepting (i.e. if the node is a left or right successor of the root of the tree).
	 * @return True if the node is accepting, false otherwise.
	 */
	bool is_accepting();

	/**
	 * Used to find the acceptance of a phrase(of which the node is a prefix) searching through this node ancestors.
	 * @param phrase	The phrase we want to verify.
	 * @param truth		Used to store the result of the search.
	 * @return			True if the phrase was found and the result stored in truth, false otherwise.
	 */
	bool is_accepting(vector<symbol_> phrase, bool& truth);

	/**
	 * Used to find the acceptance of a phrase, given a prefix of it which will be used in conjunction with the suffixes of this node's ancestors.
	 * @param prefix	The prefix used to "arrive" to this node.
	 * @param phrase	The phrase we want to verify.
	 * @param truth		Used to store the result of the search.
	 * @return			True if the phrase was found and the result stored in truth, false otherwise.
	 */
	bool is_accepting(vector<symbol_> prefix, vector<symbol_> phrase, bool& truth);

	/**
	 * Set the charaterizing phrase for this state.
	 * #param phrase	The phrase to set.
	 */
	void set_phrase(vector <symbol_> phrase);

	/**
	 * Return this state's characterizing phrase.
	 * @return	The characterizing phrase.
	 */
	vector<symbol_> get_phrase();

	/**
	 * Get the block root of this node.
	 * Given that a block is a branch of only non-final nodes this will launch an exception if this node is final.
	 * @return		The block root of this node.
	 */
	DiscrimNode* get_block_root();

	/**
	 * Get the lowest common discriminator between this node and the one passed by argument.
	 * That is the only registered proof that the 2 nodes are different
	 * (because their output functions give opposite results for their lowest common discriminator by definition).
	 */
	DiscrimNode* get_lowest_common_discriminator(DiscrimNode* other_node);

	/**
	 * Get all leaves under this node (only this node if it is a leaf).
	 * @return	A vector containing all the leaves.
	 */
	vector<DiscrimNode*> get_all_leaves();

	/**
	 * Destroy this node and all its successor except for the leaves.
	 * Useful for the finalization of nodes.
	 */
	void destroy_all_but_leaves();

	/**
	 * Return a particoular string that sums up this node in terms of its and its ancestors phrases.
	 * Useful for making the dot file, because nodes needs to be identified unambiguously by a string in that case,
	 * thus only the use of the node's phrase wouldn't suffice (different nodes may have the same phrase).
	 */
	string get_ancestors_string();
};

#endif /* DISCRIMNODE_H_ */
