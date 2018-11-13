/*
 * tttdfa.h
 *
 *  Created on: 07 set 2018
 *      Author: marco
 */

#ifndef TTTDFA_H_
#define TTTDFA_H_

#include <list>
#include <utility>

#include "dfa.h"
#include "discrimnode.h"
#include "tttstatemap.h"

class TTTDfa : public Dfa<list<DfaState>*>{
private:

	list<DfaState> states_;		/*!< real container, it will be eliminated in the future */
	DfaState* span_tree_;		/*!< root of the span tree of the TTT algorithm */
	DiscrimNode* discrim_tree_;	/*!< root of the discrimination tree of the TTT algorithm */

	DiscrimNode* utility_node_; /*!< used to memorize the node reached in the current sifting operation */
	map<vector<symbol_>, pair<DfaState*, DiscrimNode*>> span_disc_link_;	/*!< links between states of the span tree and discrimination tree leafs */
	TTTStateMap non_tree_transitions_;	/*!< a single fake node pointed by all those DfaState that have non (span) tree transition */
	map<vector<symbol_>, bool> lost_queries_;	/*!< set of queries which are lost as consequence of finalization */

public:
	/**
	 * It verify whether or not a phrase is (currently) accepted by the dfa hypothesis.
	 * It should be noted that this method uses only the span tree (the hypothesis),
	 * thus it may give false information regarding the target language
	 * (stored in the discrimination tree but not in the span tree yet).
	 */
	bool membership_query(vector<symbol_> phrase) const;

	/**
	 * Implements the Dfa's container-like behavior.
	 * @return An iterator to the first DfaState.
	 */
	list<DfaState>::iterator begin();

	/**
	 * Implements the Dfa's container-like behavior.
	 * @return An iterator to the end of the Dfa.
	 */
	list<DfaState>::iterator end();

	/**
	 * Constructor.
	 * @param alph	The alphabet of the target language.
	 * @param first_membership_queries	A vector containing the boolean value of all the strings made by only one alphabet symbol.
	 * 	They must be in the same order as the alphabet, and another final one must be added for the epsilon string.
	 */
	TTTDfa(vector<symbol_> alph, vector<bool> first_membership_queries);

	/**
	 * Destructor. It deletes every state of the span and discrimination tree (which are dynamically allocated).
	 */
	~TTTDfa();

	/**
	 * Returns the transitions that must be sift, either because they are non-existent or because they point to too shallow nodes,
	 * (a transition should point either to a block root or a leaf).
	 * @param hard_sift		True if transitions must be sifted to a leaf node (Ob. Pack), false otherwise(TTT).
	 * @return				A vector of transition represented as such a tuple: <state_phrase, transiction_symbol, hard_sift> (the last parameter is needed for the TTT).
	 */
	vector<std::tuple<vector<symbol_>, symbol_, bool>> get_transitions_to_sift(bool hard_sift);

	/**
	 * Sets the utility_node (needed for sifting) pointing at the correct node of the discrimination tree.
	 * @param prefix 		The characterizing phrase of the state the transition starts from.
	 * @param transition	The symbol associated to the transition.
	 * @param suffix		The suffix (of the node currently pointed by the transition), for which a membership query must be asked.
	 * @param hard_sift		True if this transition must be sifted until it points a leaf, false otherwise.
	 */
	void init_sifting(vector<symbol_> prefix, symbol_ transition, vector<symbol_>& suffix, bool hard_sift);

	/**
	 * A single step in sifting a transition.
	 * @param suffix		The suffix needed to ask the membership query to sift further (i.e. the suffix of node currently pointed by utility_node).
	 * @param child			Whether the membership query from the previous step returned true or false (i.e. if we must chose the 0- or the 1- branch).
	 * @param hard_sift		Whether the sifting must continue until a leaf or a block root is reached.
	 */
	bool sift_step(vector<symbol_>& suffix, bool child, bool hard_sift);

	/**
	 * Close a single transition, adding a new state if needed.
	 * @param state_phrase	The characteristic phrase of the state the transition starts from.
	 * @param transition	The symbol associated to the transition.
	 * @param child			The 0- or 1- child of the utility_node, which will be the arriving state for the transition (If NULL we reached a new state).
	 */
	void close_transition(vector<symbol_> state_phrase, symbol_ transition, bool child);

	/**
	 * Uses the information in the span and discrim tree to make a container of DfaState (basically doubling the span tree).
	 * It should be deleted after implementing a container-like behaviour that work directly on the span tree.
	 */
	void make_hypotesis();


	//void handle_counterexample(vector <symbol_> cntr_ex);

	/**
	 * Given a counterexample it goes on until a Rivest-Shapire subdivision is found (prefix,symbol,suffix)
	 * or the counterexample is correctly accepted/rejected (thus being a counterexample no longer).
	 * @param cntr_ex		The counterexample to correctly recognize.
	 * @param cntr_truth	True if the counterexample should be accepted, false otherwise.
	 * @return				True if the counterexample is still not correctly recognized, false otherwise.
	 */
	bool handle_counterexample(vector <symbol_> cntr_ex, bool cntr_truth/*, vector <symbol_>& prefix, symbol_& transition*/);

	/**
	 * Searches for blocks that can be split (having different states pointing to different blocks for the same symbol).
	 * If a split can be done the utility_node will point to the block root of the block to split.
	 * @param prefixes_to_verify		It will contain a set of prefixes that will be concatenated with the suffix to ask membership queries needed for the split.
	 * @param prefixes0					Same as prefixes_to_verify but it contains all the prefixes we already know make a non accepted phrase with the suffix.
	 * @param prefixes1					Same as prefixes0 except this time the phrases are accepted.
	 * @param suffix					It will contain the discriminator (suffix) that can split the given block.
	 * @return							Whether or not a block to split is found.
	 */
	bool try_single_finalization(vector<vector<symbol_>>& prefixes_to_verify, vector<vector<symbol_>>& prefixes0, vector<vector<symbol_>>& prefixes1, vector<symbol_>& suffix);

	/**
	 * Splits a block into 2 smaller blocks finalizing the former block root.
	 * It must be used only if a previous call to try_single_finalization returns true.
	 * @param leaf_quesies		A set of pair prefix-bool such that bool = membership_query(prefix + suffix).
	 * @param suffix			The suffix to be added to every prefix
	 */
	void split_block(vector<pair<vector<symbol_>, bool>> leaf_queries, vector<symbol_> suffix);

	/**
	 * Return true if the Dfa is deterministic thus an hypothesis can be made.
	 */
	bool is_deterministic();

	/**
	 * continua da qui.
	 */
	bool know_phrase_for_sure(vector<symbol_> phrase, bool& x);

	void print_span() const;

	void print_disc() const;

	void print() const;

	void print_ttt_dot(string name, string path);

private:
	DfaState* set_final_transition(DfaState* start_state, symbol_ transition, DiscrimNode* father, bool child);

	void set_non_tree_transition(DfaState* state, symbol_ transition, DiscrimNode* disc_node);

	pair<DfaState*, DiscrimNode*> get_state_leaf_link(vector<symbol_> phrase) const;

	DfaState* next_span_state(DfaState* start, symbol_ transition, bool strict = true) const;

	DiscrimNode* next_disc_node(DiscrimNode* start, symbol_ transition) const;

	bool test_closure(DfaState* start, list<symbol_> discriminator, bool expect, bool strict = true);

	void split_state(DfaState* start_state, symbol_ transition, vector<symbol_> discriminator, bool state_position, bool strict = true);

	void mark_nodes(vector<pair<vector<symbol_>, bool>> leaf_queries, set<DiscrimNode*>& mark_0, set<DiscrimNode*>& mark_1);

	vector<vector<DiscrimNode*>> group_leaves_into_blocks();

	void update_lost_queries(vector<pair<vector<symbol_>, bool>> leaf_queries, set<DiscrimNode*> mark_0, set<DiscrimNode*> mark_1);
};

#endif /* TTTDFA_H_ */
