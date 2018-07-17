#ifndef ANGLUINDFA_H_
#define ANGLUINDFA_H_

#include <list>
#include <vector>
#include <map>
#include <string>
#include "dfa.h"
#include "concretedfa.h"

using namespace std;

class AngluinDfa : public Dfa<vector<DfaState>*>{
private:

	/**
	 * Vector of prefixes of Angluin's S-table, expressed as vectors of symbols (string) of our alphabet.
	 */
	list <list<string>> S;

	/**
	 * Vector of prefixes of Angluin's Sa-table.
	 */
	list <list<string>> Sa;

	/**
	 * Vector of suffixes of Angluin's S-table and Sa-table.
	 */
	vector <list<string>> E;

	/**
	 * Table S of Angluin's algorithm.
	 * It contains the values of the discrete T function.
	 * The value inside look_up_s[i][j] is true if the concatenation of S[i] and E[j] belongs to our language,
	 * false otherwise.
	 */
	list <vector<int>> look_up_s;

	/**
	 * Table Sa of Angluin's algorithm.
	 * It contains the values of the discrete T function.
	 * The value inside look_up_s[i][j] is true if the concatenation of Sa[i] and E[j] belongs to our language,
	 * false otherwise.
	 */
	list <vector<int>> look_up_sa;

	/**
	 * Structure used to store rows that will be added to S after the oracle respond to some membership queries.
	 */
	list <list<string>> newS;

	/**
	 * Structure used to store rows that will be added to Sa after the oracle respond to some membership queries.
	 */
	list <list<string>> newSa;

	/**
	 * Structure used to store columns that will be added to E after the oracle respond to some membership queries.
	 */
	vector <list<string>> newE;

	/**
	 * Structure used to store rows that will be added to look_up_s after the oracle respond to some membership queries.
	 */
	list <vector<int>> new_look_up_s;

	/**
	 * Structure used to store rows that will be added to look_up_sa after the oracle respond to some membership queries.
	 */
	list <vector<int>> new_look_up_sa;

	/* New code here */
	vector<vector<vector<symbol_>>> state_to_state_table_;
	vector<DfaState> state_table_;
	/*****************/

	/*
	 * Clears all the "new" vectors (newS, newSa, new_look_up_s and so on).
	 */
	void clear_new_tables();

	/**
	 * Returns true if the 2 states (rows) are in the correct order, false otherwise.
	 * State 1 and state 2 are seen as 2 binary number, having the more significant bits at the end of the vector.
	 * In this way rows representing the same state will be contiguous.
	 */
	bool correct_order(vector <int> state1, vector <int> state2);

	/**
	 * Returns -1 if the 2 states are equal, ie each and every
	 * corresponding element has the same value for both vectors.
	 * Otherwise returns the index of the first different element found.
	 */
	int equal_states(vector<int> state1, vector<int> state2) const;

	/**
	 * Returns true if the 2 lists represent the same sequence of symbols.
	 */
	bool equal_lists(list <string> a, list <string> b) const;

	/**
	 * Returns the index of the element of S, Sa or E (or others) containing the phrase passed.
	 * Returns -1 if s is not found.
	 * @param S_Sa_E 	the vector where the phrase will be searched.
	 * @param phrase 	the phrase to be searched.
	 */
	int get_index(list <list<string>> S_Sa_E, list <string> phrase) const;

	/*
	 * If there is a couple prefix-suffix in S or Sa whose concatenation is equal to
	 * one phrase returns the value stored for that couple in look_up_s or look_up_sa
	 * (ie if we already know if the phrase belongs to our language).
	 * Returns -1 otherwise (unknown).
	 * Used to avoid asking useless membership queries to the oracle.
	 * @param phrase	phrase to search.
	 */
	int contain_phrase(list <string> phrase);

	/**
	 * Insert a new row in S and look_up_s or in Sa and look_up_sa,
	 * in the first correct position
	 * (rows representing the same state must be contiguous).
	 * @param table 	string "S" or "Sa".
	 * @param prefix 	the phrase to be inserted in S or Sa.
	 * @param queries 	the sequence of bool values for each element in E
	 */
	void insert_row(string table, list<string> prefix, vector<int> queries);

	/**
	 * Deletes a row from Sa and look_up_sa if it is already in S and look_up_s.
	 * @param prefix	prefix of the row to be deleted.
	 */
	void delete_row(list<string> prefix);

public:

	/**
	 * Returns true if the dfa is consistent, false otherwise.
	 * If the dfa is found inconsistent, it will return some phrases and
	 * will wait for the result of the membership queries of those phrases.
	 * @param phrases_to_verify		the vector which will contain the phrases to verify.
	 */
	bool get_inconsistent_phrases(map <list<string>, bool>* phrases_to_verify);

	/**
	 * Return true if the dfa is closed, false otherwise.
	 * If the dfa is not closed it will return some phrases and
	 * will wait for the result of the membership queries of those phrases.
	 * @param phrases_to_verify		the vector which will contain the phrases to verify.
	 */
	bool get_open_phrases(map <list<string>, bool>* phrases_to_verify);

	/**
	 * Given a counterexample returns the phrases that must be verified
	 * in order to modify the dfa to accept/reject the counterexample.
	 * @param phrases_to_verify 	the vector which will contain the phrases to verify.
	 * @param counterexample		the counterexample.
	 */
	void get_phrases_from_counterexample(map <list<string>, bool>* phrases_to_verify, vector<string> counterexample);

	/*
	 *Adds a new column to the look_up_s and look_up_sa tables and a new element to E.
	 *@param verified_phrases		the phrases passed by get_inconsistent_phrases.
	 *@param mem_query_res			the results of the membership queries.
	 */
	void extend_column(map <list<string>, bool>* mem_query_res);

	/**
	 * Adds one or more new rows to look_up_s and look_up_sa and one or more new elements to S and Sa.
	 *@param verified_phrases		the phrases passed by get_open_phrases or get_phrases_from_counterexample.
	 *@param mem_query_res			the results of the membership queries.
	 */
	void extend_rows(map <list<string>, bool>* mem_query_res);

	/**
	 * Returns a concrete dfa generated from the look_up_s
	 * and look_up_sa table and S and Sa vectors.
	 */
	ConcreteDfa* gen_concrete_dfa() const;

	/**
	 * Prints the look_up_s and look_up_sa tables.
	 */
	void print();

	/**
	 * Prints the new_look_up_s and new_look_up_sa tables.
	 */
	void print_new_tables();

	/**
	 * Return true if the phrase belongs to the language of the dfa, false otherwise.
	 * Badly implemented, it should not be used if the dfa is not closed or not consistent.
	 * @param phrase	the phrase to verify.
	 */
	virtual bool membership_query(vector<string> phrase) const;

	/**
	 * Constructor
	 * @param alphabet		the alphabet of the language we want to find.
	 * @param first_queries	each element indicates whether or not the corresponding symbol of the alphabet is a valid phrase. It's last element indicates whether or not the empty string is a valid phrase, so it must have the dimension of the alphabet +1.
	 */
	AngluinDfa(vector <string> alphabet, map <list<string>, bool>* first_queries);


	/* New code here */

public:
	vector<DfaState> :: iterator begin(){
		return state_table_.begin();
	};

	vector<DfaState> :: iterator end(){
		return state_table_.end();
	};

	void print_state_table();

protected:
	vector<set<list<symbol_>>> group_symbols() const;

	vector<set<list<symbol_>>> sort_states(vector<set<list<symbol_>>> states, vector<vector<symbol_>>& sorted_phrases);

	int get_start_index(vector<set<list<symbol_>>> states);

	void update_state_table();

	DFA_STATE_ get_index_from_transiction(set<list<symbol_>> current_state, symbol_ sym, vector<set<list<symbol_>>> all_states);
};
#endif /* ANGLUINDFA_H_ */
