/*
 * bluefringe.h
 */

#ifndef BLUEFRINGE_H_
#define BLUEFRINGE_H_

#include <string>
#include <vector>

#include "redbluedfa.h"
#include "utilities.h"

#define ND 	 numeric_limits<int>::max()
#define MINF numeric_limits<int>::min()

using namespace std;


/*! \class BlueFringe
    \brief Class for BlueFringe inference algorithms.

    Class for all the altgorithms which use a BlueFringe Framework.
 */
class BlueFringe
{
protected:

	// EXAMPLES
	char *path_samples_;									/*!< Path of example file (training data) */

	vector<string> alphabet_;


	// STATISTICS
	int num_actual_merge_;								/*!< Number of merge actuated by EDSM */
	int num_heuristic_merge_valued_;				/*!< Number of merge calculetd for evalution of heuristic by EDSM */
	vector<int> fringe_size_[2];						/*!< Number of red (0) and blue (1) states*/
	int while_count_;											/*!< Iterations of EDSM */



	/**
	 * Read positive and negative samples, and copy them inside 'positive' and 'negative' vector in parameters.
	 * @param positive
	 * @param dim_positive
	 * @param negative
	 * @param dim_negative
	 */
	void read_samples(vector<string>* &positive, int* dim_positive, vector<string>* &negative,  int *dim_negative,int* &wp, int* &wn);


	// TODO: Aggiornare la funzione per la creazione di un PTA
	//RedBlueDfa* build_pta(string* positive, int dim_positive);

	/**
	 * Build an APTA (Augmented Prefix Tree Acceptor) with positive and negative samples, extracted by read_samples().
	 * @param positive
	 * @param dim_positive
	 * @param negative
	 * @param dim_negative
	 * @return A pointer to built APTA.
	 */
	RedBlueDfa* build_apta(const vector<string>* positive, const int dim_positive, const vector<string>* negative, const int dim_negative);


	/**
	 * Perform a merge operation between state q1 and state q2, for DFA dfa1.
	 * @param dfa1
	 * @param q1
	 * @param q2
	 */
	void merge(RedBlueDfa* dfa1, int q1, int q2);

	/**
	 * Perform a recursive operation of fold, starting from the states to merge, inside "originale" dfa.
	 * @param originale
	 * @param q1
	 * @param q2
	 */
	void fold(RedBlueDfa* originale, int q1, int q2);

	/**
	 * Promote the state q, from blue to red state. Delete the state from the blue state list.
	 * @param dfa1
	 * @param q
	 */
	void promote(RedBlueDfa* dfa1, int q);


	/**
	 * Return a score for a dfa. It's calculated by heuristic emerged during Abbadingo Competition (1998).
	 *
	 * @param dfa1
	 * @param positive
	 * @param dim_positive
	 * @param negative
	 * @param dim_negative
	 * @return A score for a DFA, greater is better.
	 */
	virtual int  merge_heuristic_score(RedBlueDfa* dfa1, vector<string>* positive, int dim_positive, vector<string>* negative, int dim_negative, int* wp , int* wn) = 0;


	virtual double merge_heuristic_score(double error_rate_before, double error_rate_after, int dim_strings, double alpha, int earn_states) = 0;

	virtual double merge_heuristic_score(double error_rate_before, double error_rate_after, int dim_strings, double alpha, double earn_states) = 0;


	/**
	 * Update the 'dfa1' adding new blue states.
	 * Every state, that is achievable from a red state is promoted to blue,
	 * unless arrive state is already red.
	 * @param dfa1
	 */
	void nuoviBlu(RedBlueDfa* dfa1);


	/**
	 * Set the type state to 'DEL' for that states which are
	 * unreachable from every other state: fanin=0.
	 * @param dfa1
	 */
	void eliminaStati(RedBlueDfa* dfa1);

	/**
	 * Set a new record of values that are the fringe size of blue states, and
	 * the number of red states. Every time the function is called a new
	 * snapshot is taken.
	 * @param r
	 * @param b
	 */
	void set_fringe_size(int r, int b);

	/**
	 * Print all the snapshots taken, during EDSM process, about the fringe size.
	 */
	void print_fringe_size();




public:

	/**
	 * Instance an object with all the members and methods for EDSM inference process.
	 * @param path It's the path where find positive and negative samples
	 */
	BlueFringe(const char* path);								// Take the path where find samples

	/**
	 * Destroy and EDSM object, freeing the memory.
	 */
	virtual	~BlueFringe();


	/**
	 * Start an EDSM inference process.
	 * @param path It's the base path where create all the output files of EDSM.
	 * @param exec_time time elapsed, by default is 0, DO NOT pass it as argument if not interested
	 * @return Inferred DFA
	 */
	virtual 	Dfa* run(string path, double exec_time=0) = 0;						// Argument is the base path where create files



	/**
	 * It's a function with statistical finality.
	 * Return the number of merges already performed to the actual stage of EDSM process.
	 * @return Number of merges already performed to the actual stage of EDSM process.
	 */
	int  get_actual_merge();

	/**
	 * It's a function with statistical finality.
	 * Return the number of all simulated merges for heuristic evaluation, from first to actual stage.
	 * @return Number of all simulated merges for heuristic evaluation to actual stage.
	 */
	int  get_heuristic_merge();

	/**
	 * It's a function with statistical finality.
	 * Return the number of iteration of EDSM: it's the number of times that EDSM finded that
	 * there was already blue states.
	 * @return
	 */
	int  get_while_count();

};



#endif /* BLUEFRINGE_H_ */
