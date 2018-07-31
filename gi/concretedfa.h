/*
 * concretedfa.h
 */


#ifndef CONCRETE_DFA_H_
#define CONCRETE_DFA_H_

#include <vector>
#include <set>
#include <map>
#include <limits>

#include "dfa.h"
#include "exceptions.h"
#include "utilities.h"

// Type of states
#define DFA_STATE_UNREACHABLE 3							/*!< Unreachable state from other states. Usually is a state to be delete. */
#define DFA_STATE_NON_ACCEPTING 2						/*!< Non accepting state */
#define DFA_STATE_ACCEPTING 1							  /*!< Accepting state */
#define DFA_STATE_REJECTING 0							  /*!< Rejecting state */

#define ND numeric_limits<int>::max()				/*!< Usually adopted for a non initialized state. */

using namespace std;

class DfaSim;

class ConcreteDfa : public Dfa<vector<DfaState>*>{

protected:

  //******** DATA MEMBERS: ********
  vector<int> accepting_states_;      /*!< List of accepting states */
  vector<map<symbol_,int>> ttable_;			  	/*!< Transition table */

  //******** CONSTRUCTORS: ********
  /**
   * Make an instance of new dfa with default start state to 0.
   * @param n_state	Number of states.
   * @param alf		Alphabet symbols.
   */
  ConcreteDfa(const int n_state, const vector<symbol_> alf);

  /**
   * Make an instance of new dfa. Give possibility to set the start state.
   * @param n_state	Number of states.
   * @param alf		Alphabet symbols.
   * @param s_state	Start state.
   */
  ConcreteDfa(const int n_state, const vector<symbol_> alf, const int s_state);
  /**
   * Make an instance of new dfa.
   * Set tra transistion table making a copy of "tt_copy" passed as argument.
   * @param n_state	Number of states.
   * @param alf		Alphabet symbols.
   * @param s_state	Start state.
   * @param tt_copy	Reference to extern transition table to copy inside current dfa.
   * @param accepting_states Accepting states.
   */
  ConcreteDfa(const int n_state, const vector<symbol_> alf, const int s_state, vector<map<symbol_,int>> tt_copy, vector<int> accepting_states );

  //******** METHODS: ********
  /**
   * Set the transition table ("ttable") reference to an extern transition table passed as argument.
   * @param ext_ttab.
   */
  void 	set_ttable(const vector<map<symbol_,int>> ext_ttab);

  /**
   * Set a single value "v" for ttable entry for state "i", with transition symbol"j".
   * @param i state from which the transition starts.
   * @param j	transition symbol.
   * @param v	Value to set.
   */
  void 	set_ttable_entry(int i, symbol_ j, int v);

  /**
   * Insert a particular value in accepting_states_
   * @param state
   * @param value
   */
  void set_accepting_states_entry(int state, int value);

  /**
   * Create a ttable from a sequence of numbers.
   * @param sequence The sequence of numbers from which create the table. Dimension of @p sequence have to be a multiple of dim_alphabet+1. Furthermore  (sequence.size() / n_col) have to be equal to num_states.
   */
  void set_ttable_from_sequence(const vector<int> &sequence);

  /**
   * Set a state as accepting.
   * @param state_to_mark.
   */
  void set_accepting_state(int state_to_mark);

  /**
   * Set a state as rejecting.
   * @param state_to_mark.
   */
  void set_rejecting_state(int state_to_mark);

  /**
   * Make an equivalence query, that return true if the current dfa is equivalent to dfa "dfa_hp", argument of the function.
   * Otherwise return false and detect a witness, that is a counterexample that distinguishes the two dfa.
   * @param dfa 	Dfa to compare with the current dfa.
   * @param witness_results 	A vector<symbol_> in which save the witness.if miss is NULL for default,it means that the client isn't interested in witness but in ceck equivalence alone.
   * @return true if the two dfas are equivalents, false otherwise.
   */
  bool	 equivalence_query(ConcreteDfa* dfa_hp, vector<symbol_> *witness_results=NULL);

  /**
   * Fills a table with the "Table Filling Algorithm", suited for finding the equivalent/distinct states,
   * and also for generating a witness between different DFAs.
   * The Table considered is only the upper triangular matrix, that can be saved in a linear array.
   * @return A table saved in a linear array, with a list of equivalent/different states.
   */
  vector<symbol_>	table_filling() const;

  /**
   * Make a new dfa from the union of current dfa and "dfa_hp".
   * The first states are from current dfa, last states from "dfa_hp". The total number of states are sum of the number of states for the 2 dfa.
   * The start state is assumed to be the one of the method's caller.
   * Note: DO NOT USE update_state_table on the dfa returned by this procedure
   * @param dfa_hp Dfa to add to the current dfa.
   * @return Pointer to the union dfa of two dfa.
   */
  ConcreteDfa* unionDFA(ConcreteDfa* s) const;

  /**
   * Create a list of states and corrispective equivalent states.
   * @param distincts A table build with the Table Filling Algorithm.
   * @return Every vector is a list of equivalent state for the state associate to that vector.
   */
  vector<vector<int>> 		equivalent_states_list_from_table(vector<symbol_> distincts);

  /**
   * Make a conterexample from a table build with Table Filling Algorithm using the union dfa of the two dfa to compare.
   * @param distinct Table build with Table Filling Algorithm.
   * @param start_state_dfa_hp Index of the first state of dfa_hp inside the union dfa.
   * @return A witness that distinguishes the two dfa.
   */
  vector<symbol_>		witness_from_table(vector<symbol_> distinct, int start_state_dfa_hp);

  /**
   * Utility function to determine the exponent of Sigma in w-method, which is the depth
   * of the cover set.
   * @param set the set to examine
   * @return max depth of a set.
   */
  size_t get_set_depth(vector<vector<symbol_>> set) const;

  /**
   * Return a cover set of symbol_s for current DFA.
   * @return A vector of phrases (i.e. vector<symbol_>).
   */
  vector<vector<symbol_>>		get_cover_set() const;

  /**
   * Return a characterization set of symbol_s for current DFA.
   * @return A vector of phrases (i.e. vector<symbol_>).
   */
  vector<vector<symbol_>> 	get_characterization_set() const;

  /**
   * Return an augmented characterization set of symbol_s for current DFA.
   * @param sigma_exponent.
   * @param aug_characterization_set.
   * @return A vector of phrases (i.e. vector<symbol_>).
   */
  vector<vector<symbol_>> 	get_augmented_characterization_set(int sigma_exponent, vector<vector<symbol_> >& aug_characterization_set)const;

public:
  friend class BlueFringe;
  friend class OldAngluinDfa;
  friend class AngluinDfa;
  friend class SillyOracle;
  //******** CONSTRUCTORS: ********

  /**
   * Make an instance of null dfa.
   */
  ConcreteDfa();

  /**
   * Constructor for make a copy of a dfa "d1".
   * @param d1	Dfa to copy.
   */
  ConcreteDfa(const ConcreteDfa &d1);

  /**
   * Definition of assignement operator=.
   * @param d1	Dfa to copy.
   */
  ConcreteDfa &operator=(const ConcreteDfa &d1);

  /**
   * Definition of equivalence operator==.
   * @param d1	Dfa to compare.
   */
  bool operator==(const ConcreteDfa &d1) const;

  /**
   * Definition of minor operator<.
   * @param d1	Dfa to compare.
   * @return true if calling DFA has less states than DFA @p d1.
   */
  bool operator<(const ConcreteDfa &d1) const;

  /**
   * Destroy a DFA object, freeing the memory.
   */
  ~ConcreteDfa();

  //******** METHODS: ********
  /**
   * Read a dfa from a file.
   * @param file_name Path of the dfa.
   * @return Dfa read from file.
   */
  static ConcreteDfa read_dfa_file(const symbol_ file_name);

  /**
   * Return accepting states.
   * @return accepting_states_.
   */
  const vector<int> get_accepting_states() const;

  /**
   * Returns true if current state is accepting.
   * @param curr_state to test if is accepting.
   * @return true if curr_state is an accepting one.
   */
  bool is_accepting(int curr_state) const;

  /**
   * Return a reference to ttable_.
   * @return Pointer to ttable.
   */
  vector<map<symbol_,int>> 	get_ttable() const;

  /**
   * Get transition from state i with symbol_ j.
   * @param i state to start.
   * @param j symbol_ to process.
   * @return
   */
  int 	get_ttable(int i, symbol_ j) const;


  /**
   * Each ttable element is extracted in a random way (uniform numbers in [0 num_states-1] instead accepting/rejecting uniform numbers in [0 1] ). Thus the ttable (transition table) is modified.
   * @return A symbol_ representing all the number extracted.
   */
  symbol_ random_ttable();

  /**
   * Minimizes the current dfa exploiting the Table-Filling algorithm.
   * @return Pointer to a new instance of dfa, that is the minimized current dfa.
   */
  ConcreteDfa*  	minimize_TF() const;

  /**
   * Print the transition table of current dfa. Before the transition table print the title passed as parameter.
   * @param title Title printed before the transition table.
   */
  void 	print_dfa_ttable(symbol_ title) const;

  /**
   * Print a dot file for the current dfa, with title "title", in the path "file_path".
   * @param title	Title printed before the transition table.
   * @param file_path Path where make a dot file.
   */
  void    print_dfa_dot(symbol_ title, const char *file_path);

  /**
   * Print a DFA in a text file; adopted format is the same used for reading a DFA from file.
   * @ file_path A path for the new file to be created.
   */
  void 	print_dfa_in_text_file(const symbol_ file_path);

  /**
   * Get index of arrive state for dfa_symbol_ argument.
   * @param phrase String executed on dfa
   * @return Index of arrive state for "dfa_symbol_"
   */
  int		get_arrive_state(vector<symbol_> &phrase) const;

  /**
   * Make a membership query to dfa with the "phrase" symbol_ with alphabet symbol.
   * Return "true" if the arrive state for "phrase" is acceptor, else "false".
   * @param phrase A phrase (i.e. vector<symbol_>) to make a membership query.
   * @return "True" o "false" depending on the arrive state: "true" if acceptor, else if not.
   */
  bool   membership_query(vector<symbol_> phrase) const;

  /**
   * Calculate the similarity of 2 DFA's languages
   * It's target DFA that must do the calling of the method
   * If you want use W-Method for technical rasons is possible that random walk is anyway used (too big or too small test set generated with W-Method)
   * DFA aren't modified!
   * Random walk generate the test set samples (1500) both on the target and on the other dfa.
   * If W-Method is used the 2 DFA (dfa calling) and that in the parameter have to be minimized otherwise an exception is launched.
   * @throw invalidParameters    if second parameter is invalid
   * @throw wMethodTestSetTooBig if the sample generated with W-Method is too big
   * @throw mandatoryMinimalDFA  if you use W-Method and one or both dfa's aren't minimal
   * @param dfa_to_compare is the dfa to compare
   * @param method is a symbol_ indicating method generation samples to compare dfa's. Possible values are:
   *     - w-method
   *     - random-walk
   * @param stats1 It will contain statistical results (it is defined in gi_utilities.h)
   * @param stats2 It will contain statistical results if random walk is used (it is defined in gi_utilities.h)
   * @return A flag to etablish which method to generate test set was used. The flag is false if was employed W-Method, true if was used random-walk.
   */
  bool compare_dfa( ConcreteDfa *dfa_to_compare,symbol_ method,ir_statistical_measures &stats1,ir_statistical_measures &stats2);

  /**
   * Return a set of access symbol_s for all the states of DFA.
   * @return A vector of symbol_ (true symbol_!) characterizing current DFA, they adopted the alphabet.
   */
  map<int,vector<symbol_>> 	get_access_strings() const;

  /**
   * It returns a set random samples (all different) generated from current DFA.
   * They are within a map of vector symbol_; each sample (vector<symbol_>) is made up of symbols (symbol_)
   * and is associated to a number (0: negative, 1:positive).
   *
   * Based on the algorithm described in "STAMINA: a competition to encourage the development
   * and assessment of software model inference techniques", by N. Walkinshaw,
   * B. Lambeau, C. Damas, K. Bogdanov, P. Dupont. DOI 10.1007/s10664-012-9210-3
   * @param n_pos_samples Number of positive samples to be generated.
   * @param n_neg_samples Number of negative samples to be generated.
   */
  map< vector<symbol_>, int>	generate_pos_neg_samples_without_weights(int n_pos_samples,int n_neg_samples) const;

  /*
   * It returns a set of weigths paired to the samples generated yet.
   * @param samples Generated samples.
   * @param upper_bound_for_weights Upper bound for weights.
   */
  map< vector<symbol_>, int> 	generate_weights_for_pos_neg_samples(map< vector<symbol_>, int> samples, int upper_bound_for_weights);

  /**
   * Print the already generated set of random samples generated from current DFA.
   * @param samples Generated samples.
   * @param weights Generated weights.
   */
  void 	print_set_of_pos_neg_samples(map< vector<symbol_>, int> samples, map< vector<symbol_>, int> weights);

  /**
   * Write in a file an already generated set of random symbol_s accepted and rejected by the current DFA.
   * @param samples
   * @param weights
   * @param file_paht File path with generated samples.
   */
  bool 	write_existent_set_of_pos_neg_samples_in_file(map< vector<symbol_>, int> samples, map< vector<symbol_>, int> weights, const char * file_path);

  /**
   * Write in a file an already generated set of random symbol_s accepted and rejected by the current DFA.
   * @param samples
   * @param file_paht File path with generated samples.
   */
  bool 	write_existent_set_of_pos_neg_samples_in_file_without_weights(map< vector<symbol_>, int> samples, const char * file_path);

  /**
   * It generates a set of random symbol_s accepted and rejected by the current DFA and DIRECTLY write them to a file.
   * @param n_pos_samples Number of positive samples to be generated.
   * @param n_neg_samples Number of negative samples to be generated.
   * @param upper_bound_for_weights
   * @param file_paht File path with generated samples.
   */
  bool 	write_pos_neg_samples_in_file(int n_pos_samples,int n_neg_samples, int upper_bound_for_weights, const char * file_path);

  /**
   * Return a W-METHOD test set of symbol_s for current DFA.
   * @param target_dfa
   * @param sigma a boolean that if true, it tells the algoithm to include the central term sigma^k
   * in general sigma is true, it false for debugging purposes
   * @return A vector of symbol_s
   */
  vector<vector<symbol_>> 	get_w_method_test_set(ConcreteDfa* target_dfa, bool sigma=true) const;

  /**
   * Given a test set and the reference and subject dfa,
   * this function return a 9-dimensional array with all the statistics.
   * @param test_set
   * @param subject_dfa
   * @return array containing {tp,fn,tn,fp,precision,recall,f-measure,specifity,bcr}
   */
   vector<long double> get_w_method_statistics(vector<vector<symbol_>> test_set, ConcreteDfa* subject_dfa) const;

  /**
   * Print the w-method statistics
   * @param statistics
   */
  void print_w_method(vector<long double> statistics) const;


  /**
   * Print the similarity score between dfas, taking into account both linguistical
   * and structural sides.
   * @param target_dfa
   * @param sigma a boolean that if true, it tells the algoithm to include the central term sigma^k , in general sigma is true, it false for debugging purposes
   * @param eps precision of the termination condition, a by default is eps=0.0001
   * @param color if TRUE it gives label 1 to accepting states and 0 to rejecting ones.
   */
  void print_dfa_similarity(ConcreteDfa* subject_dfa, bool sigma=true, double eps=0.0001, bool color=false);

  /**
   * Returns a DfaSim object which contains all the similarity score between dfas,
   * taking into account both linguistical and structural sides.
   * @param target_dfa
   * @param print a boolean that if true permits to display the results.
   * @param sigma a boolean that if true, it tells the algoithm to include the central term sigma^k , in general sigma is true, it false for debugging purposes
   * @param eps precision of the termination condition, a by default is eps=0.0001
   * @param color if TRUE it gives label 1 to accepting states and 0 to rejecting ones.
   */
  DfaSim dfa_similarity(ConcreteDfa* subject_dfa, bool print=false, bool sigma=true, double eps=0.0001, bool color=false);




  /* Code related to the "dfa common interface" */

protected:

  vector<DfaState> state_table_;	/*!< The real container, ConcreteDfa is just a wrapper */

  /**
   * It sorts the states of the ConcreteDfa in a depth-first and lexicographical order
   */
  vector<int> sort_states(vector<vector<symbol_>>& sorted_phrases);

public:

  /**
   * Read the tables of the ConcreteDfa and update the state_table member, a vector of DfaStates.
   * The ConcreteDfa can act as a container only after this function is called.
   */
  void update_state_table();

  /**
  	* Prints all the infos about the DfaState of the Dfa in the correct order.
  	*/
  void print_state_table();

  /**
   * Implements the Dfa's container-like behavior.
   * @return an iterator to the first DfaState.
   */
  vector<DfaState> :: iterator begin();

  /**
   * Implements the Dfa's container-like behavior.
   * @return an iterator to the end of the Dfa.
   */
  vector<DfaState> :: iterator end();

};

#endif    /* CONCRETE_DFA_H_ */
