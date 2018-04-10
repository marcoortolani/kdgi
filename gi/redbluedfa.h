/*
 * redbluedfa.h
 */


#ifndef REDBLUEDFA_H_
#define REDBLUEDFA_H_
#include "dfa.h"


/*! \class redbluedfa
    \brief Class redbluedfa, inherits from class DFA.

    Class for DFA representation inside the EDSM algorithm. It add two vector, used to store which state are blue and
    which state are red. With them, it add also all the members for for their management.
*/
class RedBlueDfa: public Dfa {

private:

	vector<int>* blue_states;						/*! Blue states  */
	vector<int>* red_states;						/*! Red states   */

public:

	/**
	 * Make an instance of new dfa, with all members set to null.
	 */
	RedBlueDfa();

	/**
	 * Make an instance of RedBlueDfa from an object dfa.
	 * @param d1 Object dfa used for make the RedBlueDfa object.
	 */
	RedBlueDfa(const Dfa &d1);

	/**
	 * Constructor for make a copy of a RedBlueDfa "d1"
	 * @param d1
	 */
	RedBlueDfa(const RedBlueDfa &d1);

	/**
	 * Make an instance of new RedBlueDfa. Give possibility to set the start state.
	 * @param n_state	Number of states
	 * @param dim_alf	Size of alphabet
	 * @param alf		Alphabet symbols
	 */
	RedBlueDfa(const int n_state, const int dim_alf, const string *alf);

	/**
	 * Make an instance of new RedBlueDfa. Give possibility to set the start state.
	 * @param n_state	Number of states
	 * @param dim_alf	Size of alphabet
	 * @param alf		Alphabet symbols
	 * @param s_state	Start state
	 */
	RedBlueDfa(const int n_state, const int dim_alf, const string *alf, const int s_state);

	/**
	 * Destroy a DFA object, freeing the memory.
	 */
	~RedBlueDfa();

	/**
	 * Make an instance of dfa from the current RedBlueDfa object.
	 * @return A pointer to the new dfa.
	 */
	Dfa* to_dfa();

	/**
	 * Differently from "to_dfa()" function; it return a RedBlueDfa, without unreachable states,  adding if necessary a sink state.
	 * It adoperate the information about the red states for select the state for final dfa (use the "eliminaStati" for delete pure unreacheable states).
	 * @return A pointer to new dfa.
	 */
	RedBlueDfa* to_canonical_RedBlueDfa_from_red_states();


	int get_actual_num_states();


//EDSM

	/**
	 * Get a pointer to the vector of blue states.
	 * @return Pointer to the vector of blue states.
	 */
	vector<int>* get_blue_states() const;

	/**
	 * Get a pointer to the vector of red states.
	 * @return Pointer to the vector of red states.
	 */
	vector<int>* get_red_states() const;

	/**
	 * Add a blue state to the Vector of blue state
	 * @param blue_state_index Index of the new blue state
	 */
	void	add_blue_state(int blue_state_index);

	/**
	 * Add a red state to the Vector of blue state
	 * @param red_state_index Index of the new red state
	 */
	void 	add_red_state(int red_state_index);

	/**
	 * Remove a state from the blue states.
	 * @param blue_state_index Index of the blue state to remove.
	 */
	void 	remove_blue_state(int blue_state_index);

	/**
	 * Remove a state from the red states.
	 * @param red_state_index Index of the red state to remove.
	 */
	void 	remove_red_state(int red_state_index);

	/**
	 * Get the number of blue states.
	 * @return Number of blue states.
	 */
	int		get_num_blue_states();

	/**
	 * Get the number of red states.
	 * @return Number of red states.
	 */
	int 	get_num_red_states();

	/**
	 * Return true if blue_state_index is inside the Vector of blue states
	 * @param blue_state_index Index of blue state to find between the blue states
	 * @return True if blue_state_index is inside the Vector of blue states
	 */
	bool 	is_inside_blue_states(int blue_state_index);

	/**
	 * Return true if red_state_index is inside the Vector of red states
	 * @param red_state_index Index of red state to find between the red states
	 * @return True if red_state_index is inside the Vector of red states
	 */
	bool 	is_inside_red_states(int red_state_index);

	/**
	 * Copy in the member vector the blue states from an extern vector
	 * @param new_blue_vector Extern vector of blue states
	 */
	void	copy_blue_states(vector<int>* new_blue_vector);

	/**
	 * Copy in the member vector the red states from an extern vector
	 * @param new_red_states Extern vector of red states
	 */
	void	copy_red_states(vector<int>* new_red_states);

	/**
	 * Print the transition table with the color of the states. Before it, print the "title"
	 * @param title Title printed before the transition table
	 */
	void 	print_dfa_with_color(string title);

	/**
	 * Print the transition table with the color of the states, using the alphabet symbol. Before it, print the "title"
	 * @param title Title printed before the transition table
	 */
	void 	print_dfa_with_color_mapped_alphabet(string title);

	/**
	 * Print a dot file for the current dfa, with title "title", in the path "file_path", coloring the states.
	 * @param title	Title printed before the transition table
	 * @param file_path Path where make a dot file
	 */
	void 	print_dfa_dot(string title, const char *file_path);

	/**
	 * Print a dot file for the current dfa using the alphabet symbols, with title "title", in the path "file_path", coloring the states.
	 * @param title	Title printed before the transition table
	 * @param file_path Path where make a dot file
	 */
	void 	print_dfa_dot_mapped_alphabet(string title, const char *file_path);
};

#endif		/* REDBLUEDFA_H_ */