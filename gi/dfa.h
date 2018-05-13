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
#define DFA_STATE_NON_ACCEPTING 2					/*!< Non accepting state */
#define DFA_STATE_ACCEPTING 1							/*!< Accepting state */
#define DFA_STATE_REJECTING 0							/*!< Rejecting state */
#define DFA_STATE_UNREACHABLE 3						/*!< Unreachable state from other states. Usually is a state to be delete. */

#define ND numeric_limits<int>::max()					/*!< Usually adopted for a non initialized state. */

using namespace std;


class Dfa {

protected:
  //******** DATA MEMBERS: ********

  int	  num_states_;									/*!< Number of dfa states */
  int	  start_state_;									/*!< Index of start state */
  vector<string> alphabet_;								/*!< Alphabet symbols */
  vector<map<string,int>> ttable_;			  	/*!< Transition table */
  vector<int> accepting_states_;      /*!< List of accepting states */

  //******** METHODS: ********

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
  void 	set_alphabet(const vector<string> alf);

  /**
   * Set the transition table ("ttable") reference to an extern transition table passed as argument
   * @param ext_ttab
   */
  void 	set_ttable(const vector<map<string,int>> ext_ttab);

  /**
   * Set a single value "v" for ttable entry for "i","j"
   * @param i First coordinate
   * @param j	Second coordinate - transition symbol
   * @param v	Value to set
   */
  void 	set_ttable_entry(int i, string j, int v);

  /**
   * Set a state as accepting
   * @param state_to_mark
   */
  void set_accepting_state(int state_to_mark);

  /**
   * Set a state as rejecting
   * @param state_to_mark
   */
  void set_rejecting_state(int state_to_mark);

  /**
   * Create a ttable from a sequence of numbers
   * @param sequence The sequence of numbers from which create the table. Dimension of @p sequence have to be a multiple of dim_alphabet+1. Furthermore  (sequence.size() / n_col) have to be equal to num_states
   */
  void set_ttable_from_sequence(const vector<int> &sequence);


public:
  //******** CONSTRUCTORS: ********

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
  Dfa(const int n_state, const vector<string> alf);

  /**
   * Make an instance of new dfa. Give possibility to set the start state.
   * @param n_state	Number of states
   * @param dim_alf	Size of alphabet
   * @param alf		Alphabet symbols
   * @param s_state	Start state
   */
  Dfa(const int n_state, const vector<string> alf, const int s_state);

  /**
   * Make an instance of new dfa.
   * Set tra transistion table making a copy of "tt_copy" passed as argument.
   * @param n_state	Number of states
   * @param dim_alf	Size of alphabet
   * @param alf		Alphabet symbols
   * @param s_state	Start state
   * @param tt_copy	Reference to extern transition table to copy inside current dfa
   */
  Dfa(const int n_state, const vector<string> alf, const int s_state, vector<map<string,int>> tt_copy );

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

  //******** METHODS: ********

  /**
   * Return size of alphabet
   * @return Size of alphabet
   */
  int  	get_dim_alphabet() const;

  /**
   * Read a dfa from a file
   * @param file_name Path of the dfa
   * @return Dfa read from file
   */
  static Dfa read_dfa_file(const string file_name);

  /**
   * Return a pointer to alphabet symbols
   * @return Pointer to alphabet symbols
   */
  const vector<string>	get_alphabet() const;

  /**
   * Return accepting states
   * @return accepting_states_
   */
  const vector<int> get_accepting_states() const;

  /**
   * Returns true if current state is accepting
   * @param curr_state to test if is accepting
   * @return true if curr_state is an accepting one
   */
  bool is_accepting(int curr_state) const;

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
   * Return a reference to ttable_
   * @return Pointer to ttable
   */
  vector<map<string,int>> 	get_ttable() const;

  /**
   * Get value of ttable for index "i", "j"
   * @param i
   * @param j
   * @return
   */
  int 	get_ttable(int i, string j);

  /**
   * Make a new dfa from the union of current dfa and "dfa_hp".
   * The first states are from current dfa, last states from "dfa_hp". The total number of states are sum of the number of states for the 2 dfa.
   * @param dfa_hp Dfa to add to the current dfa
   * @return Pointer to the union dfa of two dfa
   */
  Dfa*	unionDFA(Dfa* dfa_hp);										// Return a union dfa of (this) and "dfa_hp"

  /**
   * Each ttable element is extracted in a random way (uniform numbers in [0 num_states-1] instead accepting/rejecting uniform numbers in [0 1] ). Thus the ttable (transition table) is modified.
   * @return A string representing all the number extracted
   */
  string random_ttable();

  /**
   * Minimizes the current dfa exploiting the Table-Filling algorithm
   * @return Pointer to a new instance of dfa, that is the minimized current dfa
   */
  Dfa*  	minimize_TF() const;


};

#endif    /* DFA_H_ */
