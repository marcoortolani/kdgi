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

class TTTDfa : public Dfa<vector<DfaState>*>{
private:

	vector<DfaState> states_;	/*!< real container, it will be eliminated in the future */
	DfaState* span_tree_;		/*!< root of the span tree of the TTT algorithm */
	DiscrimNode* discrim_tree_;	/*!< root of the discrimination tree of the TTT algorithm */

	bool utility_flag_; /*!< used to memorize the acceptance of the state we will reach in the current read_disc_tree operation */
	DiscrimNode* utility_node_; /*!< used to memorize the node reached in the current read_disc_tree operation */
	map<vector<symbol_>, pair<DfaState*, DiscrimNode*>> span_disc_link_;	/*!< links between states of the span tree and discrimination tree leafs */
	TTTStateMap non_tree_transitions_;	/*!< a single fake node pointed by all those DfaState that have non (span) tree transition */


public:
	bool membership_query(vector<symbol_> phrase) const;
	vector<DfaState>::iterator begin();
	vector<DfaState>::iterator end();

	TTTDfa(vector<symbol_> alph, vector<bool> first_membership_queries);
	~TTTDfa();

	DfaState* set_final_transition(DfaState* start_state, symbol_ transition, DiscrimNode* father, bool child);

	void set_non_tree_transition(DfaState* state, symbol_ transition, DiscrimNode* disc_node);

	pair<DfaState*, DiscrimNode*> get_state_leaf_link(vector<symbol_> phrase) const;

	DfaState* next_span_state(DfaState* start, symbol_ transition) const;

	DiscrimNode* next_disc_node(DiscrimNode* start, symbol_ transition) const;

	bool test_closedness(DfaState* start, list<symbol_> discriminator, bool expect);

	void split_state(DfaState* start_state, symbol_ transition, vector<symbol_> discriminator, bool state_position);

	void handle_counterexample(vector <symbol_> cntr_ex);

	vector<std::tuple<vector<symbol_>, symbol_, bool>> get_transitions_to_sift(bool hard_sift);

	bool init_sifting(vector<symbol_> prefix, symbol_ transition, vector<symbol_>& suffix, bool child, bool hard_sift);

	bool sift_step(vector<symbol_>& suffix, bool child, bool hard_sift);

	void close_transition(vector<symbol_> state_phrase, symbol_ transition, bool child);

	bool try_single_finalization(vector<vector<symbol_>>& prefixes, vector<symbol_>& suffix);

	void mark_nodes(vector<pair<vector<symbol_>, bool>> leaf_queries, set<DiscrimNode*>& mark_0, set<DiscrimNode*>& mark_1);

	void split_block(vector<pair<vector<symbol_>, bool>> leaf_queries, vector<symbol_> suffix);

	vector<vector<DiscrimNode*>> group_leaves_into_blocks();

	void make_hypotesis();

	void print_span() const;

	void print_disc() const;

	void print() const;

	void test_block_root();

	bool is_deterministic();

	void test();
};

#endif /* TTTDFA_H_ */
