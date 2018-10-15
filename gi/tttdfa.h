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

	list<DfaState> states_;	/*!< real container, it will be eliminated in the future */
	DfaState* span_tree_;		/*!< root of the span tree of the TTT algorithm */
	DiscrimNode* discrim_tree_;	/*!< root of the discrimination tree of the TTT algorithm */

	DiscrimNode* utility_node_; /*!< used to memorize the node reached in the current read_disc_tree operation */
	map<vector<symbol_>, pair<DfaState*, DiscrimNode*>> span_disc_link_;	/*!< links between states of the span tree and discrimination tree leafs */
	TTTStateMap non_tree_transitions_;	/*!< a single fake node pointed by all those DfaState that have non (span) tree transition */
	map<vector<symbol_>, bool> lost_queries_;

public:
	bool membership_query(vector<symbol_> phrase) const;
	list<DfaState>::iterator begin();
	list<DfaState>::iterator end();

	TTTDfa(vector<symbol_> alph, vector<bool> first_membership_queries);
	~TTTDfa();

	vector<std::tuple<vector<symbol_>, symbol_, bool>> get_transitions_to_sift(bool hard_sift);

	void init_sifting(vector<symbol_> prefix, symbol_ transition, vector<symbol_>& suffix, bool hard_sift);

	bool sift_step(vector<symbol_>& suffix, bool child, bool hard_sift);

	void close_transition(vector<symbol_> state_phrase, symbol_ transition, bool child);

	void make_hypotesis();

	void handle_counterexample(vector <symbol_> cntr_ex);

	bool handle_counterexample_test(vector <symbol_> cntr_ex, bool cntr_truth, vector <symbol_>& prefix, symbol_& transition);

	bool try_single_finalization(vector<vector<symbol_>>& prefixes_to_verify, vector<vector<symbol_>>& prefixes0, vector<vector<symbol_>>& prefixes1, vector<symbol_>& suffix);

	void split_block(vector<pair<vector<symbol_>, bool>> leaf_queries, vector<symbol_> suffix);

	bool is_deterministic();

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
