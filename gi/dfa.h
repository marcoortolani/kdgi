/*
 * dfa.h
 */


#ifndef DFA_H_
#define DFA_H_
#include <string>
#include <vector>
#include <set>
#include <map>
#include <limits>

#include "exceptions.h"
#include "utilities.h"

// Type of states
#define DFA_STATE_NON_ACCEPTING 2						/*!< Non accepting state */
#define DFA_STATE_ACCEPTING 1							/*!< Accepting state */
#define DFA_STATE_REJECTING 0							/*!< Rejecting state */
#define DFA_STATE_UNREACHABLE 3							/*!< Unreachable state from other states. Usually is a state to be delete. */

#define ND numeric_limits<int>::max()					/*!< Usually adopted for a non initialized state. */




typedef unsigned short int SYMBOL;

using namespace std;

//******** CLASS DATA MEMBERS CONVENTION: ********
//Data members of classes, both static and non-static,
//are named like ordinary nonmember variables,
//but with a trailing underscore.

class Dfa {

protected:
  //******** DATA MEMBERS: ********

  int** ttable_;										/*!< Transition table */
  int	  num_states_;									/*!< Number of dfa states */
  int	  start_state_;									/*!< Index of start state */
  int	  	dim_alphabet_;								/*!< Size of alphabet */
  string* alphabet_;									/*!< Alphabet symbols */
  map<string, SYMBOL> mapped_alphabet_;				/*!< Convert symbols into indices */

  //******** METHODS: ********

  /**
   * Set the transition table ("ttable") reference to an extern transition table passed as argument
   * @param ext_ttab
   */
  void 	set_ttable(int** ext_ttab);

  /**
   * Set the number of states
   * @param n
   */
  void 	set_num_state(int n);

  /**
   * Set the alphabet of dfa to alphabet in input of size "d_alf".
   * Set also "mapped_alphabet" with an index for every symbol.
   * @param alf
   * @param d_alf
   */
  void 	set_alphabet(const string* alf, const int d_alf);


  /**
   * It returns the associated letter to a SYMBOL in a mapped alphabet.
   * @param index
   */
  string	get_letter_from_mapped_alphabet(const SYMBOL index) const;

  /**
   * Get index of arrive state for dfa_string argument
   * @param dfa_string String executed on dfa
   * @return Index of arrive state for "dfa_string"
   */
  int		get_arrive_state(const vector<SYMBOL> &dfa_string) const;

  /**
   * Return a vector<SYMBOL> string from a sample made up of alphabet symbols.
   * @param str A string to be translated.
   * @return Translated string.
   */
  vector<SYMBOL>   translate_sample_using_mapped_alphabet(string str) const;

  /**
   * Make a membership query to dfa with the "str" string. Return "true" if the arrive state for "str" is acceptor, else "false".
   * @param str A string to make a membership query.
   * @return "True" o "false" depending on the arrive state: "true" if acceptor, else if not.
   */
  //bool			  	membership_query_using_mapped_alphabet(const vector<SYMBOL> &str) const;

  /**
   * Given a state, it returns a string of SYMBOLS which drives to
   * an accepting state
   * @return accepting paths
   */
  vector<vector<SYMBOL> > 			get_accepted_strings_starting_from_state(int starting_state) const;

  /**
   * Make an equivalence query, that return true if the current dfa is equivalent to dfa "dfa_hp", argument of the function.
   * Otherwise return false and detect a witness, that is a counterexample that distinguishes the two dfa.
   * @param dfa 						Dfa to compare with the current dfa
   * @param witness_results 			A vector<SYMBOL> in which save the witness.if miss is NULL for default,it means that the client isn't interested in witness but in ceck equivalence alone
   * @return true if the two dfas are equivalents, false otherwise
   */
  bool	   	     equivalence_query(Dfa* dfa_hp, vector<SYMBOL>* witness_results=NULL);

  //******** W-METHOD: ********

  /**
   * Return a cover set of strings for current DFA.
   * @return A vector of strings (i.e. vector<SYMBOL>)
   */
  vector<vector<SYMBOL> >		get_cover_set() const;

  /**
   * Return a characterization set of strings for current DFA.
   * @return A vector of strings (i.e. vector<SYMBOL>)
   */
  vector<vector<SYMBOL> > 		get_characterization_set() const;

  /**
   * Return an augmented characterization set of strings for current DFA.
   * @return A vector of strings (i.e. vector<SYMBOL>)
   */
  vector<vector<SYMBOL> > 		get_augmented_characterization_set(int sigma_exponent, vector<vector<SYMBOL> >& aug_characterization_set)const;

  /**
   * Return a W-METHOD test set of strings for current DFA.
   * @param target_dfa
   * @param sigma a boolean that if true, it tells the algoithm to include the central term sigma^k
   * in general sigma is true, it false for debugging purposes
   * @return A vectory of strings (i.e. vector<SYMBOL>)
   */
  set<vector<SYMBOL> > 		    get_w_method_test_set_mapped_alphabet(Dfa* target_dfa, bool sigma) const;


  /**
   * Return a map of access strings paired with state indexes, for all the states of DFA.
   * @return A map that pair a state index with its access string.
   */
  map<int, vector<SYMBOL> >  	get_access_strings_mapped_alphabet() const;

  /**
   * Fills a table with the "Table Filling Algorithm", suited for finding the equivalent/distinct states,
   * and also for generating a witness between different DFAs.
   * The Table considered is only the upper triangular matrix, that can be saved in a linear array.
   * @return A table saved in a linear array, with a list of equivalent/different states.
   */
  SYMBOL*		  	   	table_filling() const;

  /**
   * Create a list of states and corrispective equivalent states
   * @param distincts A table build with the Table Filling Algorithm
   * @return A pointer to the first vector<int>. Every vector is a list of equivalent state for the state associate to that vector.
   */
  vector<int>* 		equivalent_states_list_from_table(SYMBOL* distincts);

  /**
   * Make a conterexample from a table build with Table Filling Algorithm using the union dfa of the two dfa to compare.
   * @param distinct Table build with Table Filling Algorithm.
   * @param start_state_dfa_hp Index of the first state of dfa_hp inside the union dfa.
   * @return A witness that distinguishes the two dfa.
   */
  vector<SYMBOL>		witness_from_table(SYMBOL* distinct, int start_state_dfa_hp);

  /**
   * Utility function to determine the exponent of Sigma in w-method
   * @return max depth of a set
   */
  size_t get_set_depth(std::vector<std::vector<SYMBOL> > set) const;


  /**
   * Make a membership query to dfa with the "str" string. Return "true" if the arrive state for "str" is acceptor, else "false".
   * @param str A string to make a membership query.
   * @return "True" o "false" depending on the arrive state: "true" if acceptor, else if not.
   */
  bool			  	membership_query_using_mapped_alphabet(const vector<SYMBOL> &str) const;


public:

  /**
   * Make an instance of null dfa
   */
  Dfa();

  /**
   * Make an instance of new dfa with default start state to 0
   * @param n_state	Number of states
   * @param dim_alf	Size of alphabet
   * @param alf		Alphabet symbols
   */
  Dfa(const int n_state, const int dim_alf, const string *alf);

  /**
   * Make an instance of new dfa. Give possibility to set the start state.
   * @param n_state	Number of states
   * @param dim_alf	Size of alphabet
   * @param alf		Alphabet symbols
   * @param s_state	Start state
   */
  Dfa(const int n_state, const int dim_alf, const string *alf, const int s_state);

  /**
   * Make an instance of new dfa.
   * Set tra transistion table making a copy of "tt_copy" passed as argument.
   * @param n_state	Number of states
   * @param dim_alf	Size of alphabet
   * @param alf		Alphabet symbols
   * @param s_state	Start state
   * @param tt_copy	Reference to extern transition table to copy inside current dfa
   */
  Dfa(const int n_state, const int dim_alf, const string *alf, const int s_state, const int** tt_copy );

  /**
   * Constructor for make a copy of a dfa "d1"
   * @param d1	Dfa to copy
   */
  Dfa(const Dfa &d1);

  /**
   * Definition of assignement operator=
   * @param d1	Dfa to copy
   */
  const Dfa &operator=(const Dfa &d1);

  /**
   * Definition of assignement operator<
   * @param d1	Dfa to compare
   * @return true if calling DFA has less states than DFA @p d1
   */
  bool operator<(const Dfa &d1) const;

  /**
   * Destroy a DFA object, freeing the memory.
   */
  ~Dfa();

  /**
   * Read a dfa from a file
   * @param file_name Path of the dfa
   * @return Dfa read from file
   */
  static Dfa read_dfa_file(const string file_name);


  /**
   * Return size of alphabet
   * @return Size of alphabet
   */
  int  	get_dim_alphabet() const;

  /**
   * Return a pointer to alphabet symbols
   * @return Pointer to alphabet symbols
   */
  const string*	get_alphabet() const;

  /**
   * Get number of states
   * @return Number of states
   */
  int   	get_num_states() const;

  /**
   * Get index of start state
   * @return Index of start state
   */
  int   	get_start_state();

  /**
  * Get indexes of the accepting states
  * @return vector of indexes of accepting states
  */
  vector<int> 		get_accepting_states() const;

  /**
  *	Return a list of sink states for the given automata
  * else it returns numeric_limits<SYMBOL>::max()
  */
  vector<int> 			get_sink_states() const;


  /**
   * Return a reference to ttable_
   * @return Pointer to ttable
   */
  int** 	get_ttable() const;

  /**
   * Get value of ttable for index "i", "j"
   * @param i
   * @param j
   * @return
   */
  int 	get_ttable(int i, int j);

  /**
   * Set a single value "v" for ttable entry for "i","j"
   * @param i First coordinate
   * @param j	Second coordinate
   * @param v	Value to set
   */
  void 	set_ttable_entry(int i, int j, int v);

  /**
   * Create a ttable from a sequence of numbers
   * @param sequence The sequence of numbers from which create the table. Dimension of @p sequence have to be a multiple of dim_alphabet+1. Furthermore  (sequence.size() / n_col) have to be equal to num_states
   */
  void set_ttable_from_sequence(const vector<int> &sequence);

  /**
   * Each ttable element is extracted in a random way (uniform numbers in [0 num_states-1] instead accepting/rejecting uniform numbers in [0 1] ). Thus the ttable (transition table) is modified.
   * @return A string representing all the number extracted
   */
  string random_ttable();

  /**
   * Set a state as acceptor, whose index is given in the argument.
   * @param state Index of state to set as acceptor.
   */
  void 	set_acceptor_state(int state);

  /**
   * Set a state as rejector, whose index is given in the argument.
   * @param state Index of state to set as rejector.
   */
  void 	set_rejector_state(int state);

  /**
   * Make a new dfa from the union of current dfa and "dfa_hp".
   * The first states are from current dfa, last states from "dfa_hp". The total number of states are sum of the number of states for the 2 dfa.
   * @param dfa_hp Dfa to add to the current dfa
   * @return Pointer to the union dfa of two dfa
   */
  Dfa*	unionDFA(Dfa* dfa_hp);										// Return a union dfa of (this) and "dfa_hp"

  /**
   * Minimizes the current dfa exploiting the Table-Filling algorithm
   * @return Pointer to a new instance of dfa, that is the minimized current dfa
   */
  Dfa*  	minimize_TF() const;

  /**
   * Print the transition table of current dfa, using the INNER alphabet. Before the transition table print the title passse as parameter.
   * @param title Title printed before the transition table
   */
  void 	print_dfa_ttable(string title);

  /**
   * Print the transition table using the alphabet symbols. Before the transition table print the "title".
   * @param title Title printed before the transition table
   */
  void 	print_dfa_ttable_mapped_alphabet(string title);

  /**
   * Print a dot file for the current dfa, with title "title", in the path "file_path".
   * @param title	Title printed before the transition table
   * @param file_path Path where make a dot file
   */
  void    print_dfa_dot(string title, const char *file_path);

  /**
   * Print a dot file for the current dfa using the alphabet symbols, with title "title", in the path "file_path".
   * @param title	Title printed before the transition table
   * @param file_path Path where make a dot file
   */
  void 	print_dfa_dot_mapped_alphabet(string title, const char *file_path);


  /**
   * Print a DFA in a text file; adopted format is the same used for reading a DFA from file.
   * @ file_path A path for the new file to be created.
   */
  void 	print_dfa_in_text_file(const string file_path);


  /**
   * Calculate the similarity of 2 DFA's languages
   * It's target DFA that must do the calling of the method
   * If you want use W-Method for technical rasons is possible that random walk is anyway used (too big or too small test set generated with W-Method)
   * DFA aren't modified
   * Random walk generate the test set samples (1500) both on the target and on the other dfa
   * If W-Method is used the 2 DFA (dfa calling) and that in the parameter have to be minimized otherwise an exception is launched
   * @throw invalidParameters    if second parameter is invalid
   * @throw wMethodTestSetTooBig if the sample generated with W-Method is too big
   * @throw mandatoryMinimalDFA  if you use W-Method and one or both dfa's aren't minimal
   * @param dfa_to_compare is the dfa to compare
   * @param method is a string indicating method generation samples to compare dfa's. Possible values are:
   *     - w-method
   *     - random-walk
   * @param stats1 It will contain statistical results (it is defined in gi_utilities.h)
   * @param stats2 It will contain statistical results if random walk is used (it is defined in gi_utilities.h)
   * @return A flag to etablish which method to generate test set was used. The flag is false if was employed W-Method, true if was used random walk
   */
  bool compare_dfa( Dfa *dfa_to_compare,string method,ir_statistical_measures &stats1,ir_statistical_measures &stats2);

  /**
   * Make a membership query to dfa with the "str" string with alphabet symbol.
   * Return "true" if the arrive state for "str" is acceptor, else "false".
   * @param str A string to make a membership query.
   * @return "True" o "false" depending on the arrive state: "true" if acceptor, else if not.
   */
  bool   membership_query(string str) const;

  /**
   * Return a set of access strings for all the states of DFA.
   * @return A vector of string (true string!) characterizing current DFA, they adopted the alphabet.
   */
  vector<string> 				get_access_strings() const;

  /**
   * It returns a set random samples (all different) generated from current DFA.
   * They are within a map of vector string; each sample (vector<string>) is made up of symbols (string)
   * and is associated to a number (0: negative, 1:positive).
   *
   * Based on the algorithm described in "STAMINA: a competition to encourage the development
   * and assessment of software model inference techniques", by N. Walkinshaw,
   * B. Lambeau, C. Damas, K. Bogdanov, P. Dupont. DOI 10.1007/s10664-012-9210-3
   * @param n_pos_samples Number of positive samples to be generated.
   * @param n_neg_samples Number of negative samples to be generated.
   */
  map< vector<string>, int>	generate_pos_neg_samples_without_weights(int n_pos_samples,int n_neg_samples) const;

  /*
   * It returns a set of weigths paired to the samples generated yet.
   * @param samples Generated samples.
   * @param upper_bound_for_weights Upper bound for weights.
   */
  map< vector<string>, int> 	generate_weights_for_pos_neg_samples(map< vector<string>, int> samples, int upper_bound_for_weights);

  /**
   * Print the already generated set of random samples generated from current DFA.
   * @param samples Generated samples.
   * @param weights Generated weights.
   */
  void 				print_set_of_pos_neg_samples(map< vector<string>, int> samples, map< vector<string>, int> weights);

  /**
   * Write in a file an already generated set of random strings accepted and rejected by the current DFA.
   * @param samples
   * @param weights
   * @param file_paht File path with generated samples.
   */
  bool 				write_existent_set_of_pos_neg_samples_in_file(map< vector<string>, int> samples, map< vector<string>, int> weights, const char * file_path);

/**
   * Write in a file an already generated set of random strings accepted and rejected by the current DFA.
   * @param samples
   * @param file_paht File path with generated samples.
   */
  bool 				write_existent_set_of_pos_neg_samples_in_file_without_weights(map< vector<string>, int> samples, const char * file_path);

  /**
   * It generates a set of random strings accepted and rejected by the current DFA and DIRECTLY write them to a file.
   * @param n_pos_samples Number of positive samples to be generated.
   * @param n_neg_samples Number of negative samples to be generated.
   * @param file_paht File path with generated samples.
   */
  bool 				write_pos_neg_samples_in_file(int n_pos_samples,int n_neg_samples, int upper_bound_for_weights, const char * file_path);

  //******** W-METHOD: ********

  /**
   * Return a W-METHOD test set of strings for current DFA.
   * @param target_dfa
   * @param sigma a boolean that if true, it tells the algoithm to include the central term sigma^k
   * in general sigma is true, it false for debugging purposes
   * @return A vector of strings
   */
  vector<string> 		          get_w_method_test_set(Dfa* target_dfa, bool sigma) const;


  /**
   * Given a test set and the reference and subject dfa,
   * this function return a 9-dimensional array with all the statistics
   * @param test_set
   * @param subject_dfa
   * @param reference_dfa
   * @return array containing {tp,fn,tn,fp,precision,recall,f-measure,specifity,bcr}
   */
   long double* get_w_method_statistics(vector<string> test_set, Dfa* subject_dfa) const;

   /**
    * Gives the structural similarity score matrix between every pair of states of two DFAs
    * based on the Mladen Nikolic's paper "Measuring Similarity of Graph Nodes by Neighbor Matching"
    * @param subject_dfa
    * @param eps precision of the termination condition, a by default is eps=0.0001
    * @return similarity_matrix contains the similarity score of reference_dfa's state i
    *         with subject_dfa's state j. The last row, so
    *         similarity_matrix[reference_dfa->num_states][1] contains the overall
    *         structural similarity score between the two Dfas
    */
   double** neighbour_matching_structural_similarity(Dfa* subject_dfa, double eps) const;

   /**
    * Gives the structural similarity score matrix between every pair of states of two DFAs
    * based on the Mladen Nikolic's paper "Measuring Similarity of Graph Nodes by Neighbor Matching".
    * In this COLOR version we give label 1 to accepting states and 0 to rejecting ones.
    * @param subject_dfa
    * @param eps precision of the termination condition, a by default is eps=0.0001
    * @return similarity_matrix contains the similarity score of reference_dfa's state i
    *         with subject_dfa's state j. The last row, so
    *         similarity_matrix[reference_dfa->num_states][1] contains the overall
    *         structural similarity score between the two Dfas
    */
   double** neighbour_matching_structural_similarity_color(Dfa* subject_dfa, double eps) const;

  /**
   * Print the matrix containing the similarity score between pair of nodes
   * @param similarity_matrix 
   */
   void print_structural_similarity(double** similarity_matrix,int num_states_target_dfa) const;



};

#endif    /* DFA_H_ */
