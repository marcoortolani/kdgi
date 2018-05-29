/*
 * dfa.h
 */


#ifndef DFA_H_
#define DFA_H_

#include <vector>
#include <string>

using namespace std;

class Dfa{

protected:

  //******** DATA MEMBERS: ********
  int	  num_states_;									/*!< Number of dfa states */
  int	  start_state_;									/*!< Index of start state */
  vector<string> alphabet_;						/*!< Alphabet symbols */

  //******** CONSTRUCTORS: ********
  /**
   * Make an instance of new dfa with default start state to 0.
   * @param n_state	Number of states.
   * @param alf		Alphabet symbols.
   */
  Dfa(const int n_state, const vector<string> alf);

  /**
   * Make an instance of new dfa. Give possibility to set the start state.
   * @param n_state	Number of states.
   * @param alf		Alphabet symbols.
   * @param s_state	Start state.
   */
  Dfa(const int n_state, const vector<string> alf, const int s_state);

  //******** METHODS: ********
  /**
   * Set the number of states.
   * @param n.
   */
  void 	set_num_state(int n);

  /**
   * Set the alphabet of dfa to alphabet in input of size "d_alf".
   * @param alf.
   */
  void 	set_alphabet(const vector<string> alf);

public:

  /**
   * Make an instance of null dfa.
   */
  Dfa();

  virtual ~Dfa();

  //******** METHODS: ********
  /**
   * Return size of alphabet.
   * @return Size of alphabet.
   */
  int  	get_dim_alphabet() const;

  /**
   * Return a vector with alphabet symbols.
   * @return Pointer to alphabet symbols (i.e. strings).
   */
  const vector<string>	get_alphabet() const;

  /**
   * Get number of states.
   * @return Number of states.
   */
  int   get_num_states() const;

  /**
   * Get index of start state.
   * @return Index of start state.
   */
  int   get_start_state() const; 

  /**
   * Make a membership query to dfa with the "phrase" string with alphabet symbol.
   * Return "true" if the arrive state for "phrase" is acceptor, else "false".
   * @param phrase A phrase (i.e. vector<string>) to make a membership query.
   * @return "True" o "false" depending on the arrive state: "true" if acceptor, else if not.
   */
  virtual bool   membership_query(vector<string> phrase) const = 0;

};

#endif  /* DFA_H_ */