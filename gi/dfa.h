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

//#include "exceptions.h"
//#include "utilities.h"

// Type of states
#define DFA_STATE_NON_ACCEPTING 2					/*!< Non accepting state */
#define DFA_STATE_ACCEPTING 1							/*!< Accepting state */
#define DFA_STATE_REJECTING 0							/*!< Rejecting state */
#define DFA_STATE_UNREACHABLE 3						/*!< Unreachable state from other states. Usually is a state to be delete. */

#define ND numeric_limits<int>::max()					/*!< Usually adopted for a non initialized state. */

using namespace std;


template <typename T>   //T is the input type, e.g. char || string || int
class Dfa {

protected:
  //******** DATA MEMBERS: ********

  int	  num_states_;									/*!< Number of dfa states */
  int	  start_state_;									/*!< Index of start state */
  vector<T> alphabet_;								/*!< Alphabet symbols */
  vector<map<T,int>> ttable_;			  	/*!< Transition table */
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
  void 	set_alphabet(const vector<T> alf);

  /**
   * Set the transition table ("ttable") reference to an extern transition table passed as argument
   * @param ext_ttab
   */
  void 	set_ttable(const vector<map<T,int>> ext_ttab);
  
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
  Dfa(const int n_state, const vector<T> alf);

  /**
   * Make an instance of new dfa. Give possibility to set the start state.
   * @param n_state	Number of states
   * @param dim_alf	Size of alphabet
   * @param alf		Alphabet symbols
   * @param s_state	Start state
   */
  Dfa(const int n_state, const vector<T> alf, const int s_state);

  /**
   * Make an instance of new dfa.
   * Set tra transistion table making a copy of "tt_copy" passed as argument.
   * @param n_state	Number of states
   * @param dim_alf	Size of alphabet
   * @param alf		Alphabet symbols
   * @param s_state	Start state
   * @param tt_copy	Reference to extern transition table to copy inside current dfa
   */
  Dfa(const int n_state, const vector<T> alf, const int s_state, vector<map<T,int>> tt_copy );

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
  //static Dfa read_dfa_file(const string file_name);


};

#endif    /* DFA_H_ */
