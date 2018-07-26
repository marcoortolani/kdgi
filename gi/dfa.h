/*
 * dfa.h
 */


#ifndef DFA_H_
#define DFA_H_

#include <vector>
#include <string>
#include <iostream>

#include "dfastate.h"
#include "utilities.h"

#define symbol_ string
#define DFA_STATE_ int

using namespace std;

template <class Iter>
class Dfa{

public:
using basetype = typename std::iterator_traits<Iter>::value_type;
using iterator = typename basetype::iterator;

protected:

  //******** DATA MEMBERS: ********
  int	  num_states_;									/*!< Number of dfa states */
  int	  start_state_;									/*!< Index of start state */
  vector<symbol_> alphabet_;						/*!< Alphabet symbols */

  //******** CONSTRUCTORS: ********
  /**
   * Make an instance of new dfa with default start state to 0.
   * @param n_state	Number of states.
   * @param alf		Alphabet symbols.
   */
  Dfa(const int n_state, const vector<symbol_> alf);

  /**
   * Make an instance of new dfa. Give possibility to set the start state.
   * @param n_state	Number of states.
   * @param alf		Alphabet symbols.
   * @param s_state	Start state.
   */
  Dfa(const int n_state, const vector<symbol_> alf, const int s_state);

  //******** METHODS: ********
  /**
   * Set the number of states.
   * @param n.
   */
  void 	set_num_state(int n);

//TO-DO eliminare questo "public" messo per motivi di debug
public:
  /**
   * Set the alphabet of dfa to alphabet in input of size "d_alf".
   * @param alf.
   */
  void 	set_alphabet(const vector<symbol_> alf);

public:
  friend class BlueFringe;
  friend class OldAngluinDfa;
  friend class SillyOracle;
  //******** CONSTRUCTORS: ********

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
   * @return Pointer to alphabet symbols (i.e. symbol_s).
   */
  const vector<symbol_>	get_alphabet() const;

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
   * Make a membership query to dfa with the "phrase" symbol_ with alphabet symbol.
   * Return "true" if the arrive state for "phrase" is acceptor, else "false".
   * @param phrase A phrase (i.e. vector<symbol_>) to make a membership query.
   * @return "True" o "false" depending on the arrive state: "true" if acceptor, else if not.
   */
  virtual bool   membership_query(vector<symbol_> phrase) const = 0;


  /**
   * Fills a table with the "Table Filling Algorithm", suited for finding the equivalent/distinct states,
   * and also for generating a witness between different DFAs.
   * The Table considered is only the upper triangular matrix, that can be saved in a linear array.
   * @return A table saved in a linear array, with a list of equivalent/different states.
   */
  vector<symbol_>	table_filling1(Dfa* subject_dfa);



  /* Code related to the "dfa common interface" */

  virtual iterator begin() = 0;

  virtual iterator end() = 0;

  /**
   * Returns true if the 2 Dfas are identical false otherwise.
   */
  bool is_identical(Dfa* other_dfa, vector<symbol_>& phrase);

  /**
   * Returns the alphabet in lexicographical order.
   */
  vector<symbol_> sort_alphabet();

  /**
   * Return the next phrase in lexicographical order with the same length of the argument phrase or shorter.
   */
  vector<symbol_> get_next_phrase(vector<symbol_> phrase);

  /**
   * Returns the DfaState related to the phrase passed regardless of the type of container of symbol_.
   * It explores the graph symbol_ by symbol_ and DfaState by DfaState, making it usable for every Dfa.
   * A Dfa that need efficiency should override this operator.
   * It should be noted that any function defined in the Dfa generic template class that use this operator, use
   * this version whether another one is implemented in the derived class or not.
   */
  template <class SymIter>
  DfaState* operator[](SymIter phrase);

  bool find_counterexample(Dfa* other_dfa, vector<symbol_> counter_example);

  /**
    * Gives the structural similarity score matrix between every pair of states of two DFAs
    * based on the Mladen Nikolic's paper "Measuring Similarity of Graph Nodes by Neighbor Matching"
    * @param subject_dfa
    * @param eps precision of the termination condition, a by default is eps=0.0001
    * @param color if TRUE it gives label 1 to accepting states and 0 to rejecting ones.
    * @return similarity_matrix contains the similarity score of reference_dfa's state i
    * with subject_dfa's state j. The last row, so similarity_matrix[reference_dfa->num_states][1]
    * contains the overall structural similarity score between the two Dfas.      
    */
   vector<vector<double>> neighbour_matching(Dfa* subject_dfa, double eps=0.0001, bool color=false);

    /**
   * Print the matrix containing the similarity score between pair of nodes.
   * @param similarity_matrix 
   * @param num_states_target_dfa
   */
   void print_structural_similarity(vector<vector<double>> similarity_matrix) const;


};

#include "dfa.tcc"

#endif  /* DFA_H_ */
