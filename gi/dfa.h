/*
 * dfa.h
 */


#ifndef DFA_H_
#define DFA_H_

#include "dfastate.h"

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
  void 	set_alphabet(const vector<symbol_> alf);

public:
  friend class BlueFringe;
  //******** CONSTRUCTORS: ********

  /**
   * Make an instance of null dfa.
   */
  Dfa();

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
   * @param phrase The phrase on which a membership query mast be done.
   * @return "true" o "false" depending on the arrive state: "true" if acceptor, else if not.
   */
  virtual bool   membership_query(vector<symbol_> phrase) const = 0;

  /**
   * Initialize a triangular upper matrix (realized by a map of map of phrases) for the table_filling1() method,
   * used to find every couple of equivalent states.
   * @return the initialized triangular upper matrix: for each ordered couple of states will be indicated
   * if they're both accepting/rejecting or if one is accepting and the other rejecting.
   */
  map<vector<symbol_>, map<vector<symbol_>, vector<symbol_>>> init_table_filling();

  /**
   * Very similar to the homonymous method, but conversely it generate a rectangular matrix
   * that will be updated by equivalence_query() method and read by find_counterexample_from_table() method,
   * because the 2 dfa could have a different number of states and every state of the first dfa
   * must be confronted with every state of the second (while doing this in the other case
   * would have stored redundant informations).
   @return the triangular upper matrix; let's say the first dfa has 3 states: "a", "b" and "aa" and the second: "a" and "c".
   * If we want to see the equivalence of "a" and "c" there is the pseudo-code:
   * matrix = first->init_table_filling(second);
   * row = matrix["a"]; (we search for "a" first based on the ordering of the dfas);
   * discriminator = row["c"];
   * if discriminator is found the states are different because
   * if the discriminator is used on theirs respective output functions it will produce 2 different outputs.
   * If we want to test the equivalence of the 2 languages we will confront the 2 initial states.
   */
  template <class O>
  map<vector<symbol_>, map<vector<symbol_>, vector<symbol_>>> init_table_filling(Dfa<O>* second);

  /**
   * Verify if a dfa is canonical or not by returning a triangular upper matrix
   * where for each couple of state is stored a discriminator if it exists.
   * @return the triangular upper matrix; let's say the dfa has 3 states: "a", "b" and "aa" (ordered in this way).
   * If we want to see the equivalence of "a" and "b" there is the pseudo-code:
   * matrix = init_table_filling();
   * row = matrix["a"]; (we search for "a" first based on the ordering);
   * discriminator = row["b"];
   * if discriminator is found the states are different because
   * if the discriminator is used on theirs respective output functions it will produce 2 different outputs.
   */
  map<vector<symbol_>, map<vector<symbol_>, vector<symbol_>>>	table_filling1();

  /**
   * It finds a counterexample that distinguish this dfa from another one given an already generated table-filling table.
   */
  template<class O>
  vector<symbol_> find_counterexample_from_table(map<vector<symbol_>, map<vector<symbol_>, vector<symbol_>>> table, Dfa<O>* subject_dfa, vector<symbol_> phrase1 = vector<symbol_>(), vector<symbol_> phrase2 = vector<symbol_>());

  /**
   * Finds a counterexample (if one does exist) ie a phrase the other dfa recognize differently, using the table filling algorithm.
   * @param other_dfa the other dfa
   * @param counterexample a parameter passed by reference where the counterexample will be returned if it exists.
   * @return true if the 2 dfas represents the same language,
   * false otherwise in which case the counterexample will be stored in the parameter passed by reference
   */
  template<class O>
  bool equivalence_query(Dfa<O>* other_dfa, vector<symbol_>& counterexample, vector<symbol_> phrase1 = vector<symbol_>(), vector<symbol_> phrase2 = vector<symbol_>());

  /* Code related to the "dfa common interface" */

  /**
   * Needed to implement in the derived class the container-like behaviour.
   * @return Must return (being pure virtual it is not implemented in the base class) an iterator to the first state of the dfa o to its end if it is empty.
   */
  virtual iterator begin() = 0;

  /**
   * Needed to implement in the derived class the container-like behaviour.
   * @return Must return (being pure virtual it is not implemented in the base class) an iterator to the end of the dfa.
   */
  virtual iterator end() = 0;

  /**
   * Returns the alphabet in lexicographical order.
   */
  vector<symbol_> sort_alphabet() const;

  /**
   * Returns true if the 2 Dfas are identical false otherwise.
   */
  template <class O>
  bool is_identical(Dfa<O>* other_dfa, vector<symbol_>& phrase);

  /**
   * Returns the DfaState related to the phrase passed regardless of the type of container of symbol_.
   * It explores the graph symbol_ by symbol_ and DfaState by DfaState, making it usable for every Dfa.
   * A Dfa that need efficiency should override this operator.
   * It should be noted that any function defined in the Dfa generic template class that use this operator, use
   * this version whether another one is implemented in the derived class or not.
   */
  template <class SymIter>
  DfaState* operator[](SymIter phrase);

  /**
    * Gives the structural similarity score matrix between every pair of states of two DFAs
    * based on the Mladen Nikolic's paper "Measuring Similarity of Graph Nodes by Neighbor Matching"
    * @param subject_dfa
    * @param isomorphism if TRUE it divides global similarity for min(num_states_, num_states_subject_), searching for a subgraph matching
    * @param color if TRUE it gives label 1 to accepting states and 0 to rejecting ones.
    * @param eps precision of costs matrix for hungarian algorithm, by default is eps=0.0001
    * @return similarity_matrix contains the similarity score of reference_dfa's state i
    * with subject_dfa's state j. The last row, so similarity_matrix[reference_dfa->num_states][1]
    * contains the overall structural similarity score between the two Dfas.      
    */
  template <class O>
   vector<vector<double>> neighbour_matching(Dfa<O>* subject_dfa, bool isomorphism=false, bool color=false, double eps=0.0001);


    /**
   * Print the matrix containing the similarity score between pair of nodes.
   * @param similarity_matrix 
   * @param num_states_target_dfa
   */
   void print_structural_similarity(vector<vector<double>> similarity_matrix) const;

   /**
    * Print on screen the infos of all the states (It calls the print method of all DfaStates of this dfa in their order).
    */
   void print();

   /*
    * Generate the dot file of the current status of the dfa.
    * @param title the name we want to give to the dfa.
    * @param file_path the path where we want to save the file.
    */
   void print_dfa_dot(string title, string file_path);

};

#include "dfa.tcc"

#endif  /* DFA_H_ */
