/*
 * bluestar.h
 */


#ifndef BLUESTAR_H_
#define BLUESTAR_H_

#include "bluefringe.h"


using namespace std;



/*! \class blueStar
    \brief Class for Blue* inference algorithm.

    Class for Blue* inference algorithm, with all members and methods for start an inference process.
 */
class BlueStar : public BlueFringe
{
private:

	double	alpha = 0.01;
	double	delta = 0.005;
	//double	delta = 1000;


	// STATISTICS
	double error_rate_final_dfa;						/*!< Error rate for the dfa inferred respect to samples*/



	void set_acceptor_and_rejector_states(RedBlueDfa* dfa1, vector<string>* positive, const int dim_positive, vector<string>* negative, const int dim_negative, int* &wp, int* &wn);


	/**
	 * Return a score for a dfa.
	 * It's calculated by statistical heuristic of bluestar, minimimazing the ratio between second type error and earn states.
	 *
	 * @param dfa1
	 * @param positive
	 * @param dim_positive
	 * @param negative
	 * @param dim_negative
	 * @return A score for a DFA, greater is better. "MINF" value if the merge is not acceptable.
	 */
	double merge_heuristic_score(double error_rate_before, double error_rate_after, int dim_strings, double alpha, double earn_states);

	double merge_heuristic_score(double error_rate_before, double error_rate_after, int dim_strings, double alpha, int earn_states){};


	int  merge_heuristic_score(RedBlueDfa* dfa1, vector<string>* positive, int dim_positive, vector<string>* negative, int dim_negative, int* wp , int* wn){};


	/**
	 * Check if the merge is statisticaly acceptable.
	 *
	 * @param dfa1
	 * @param positive
	 * @param dim_positive
	 * @param negative
	 * @param dim_negative
	 * @return Return if a merge is statistically acceptable.
	 */
	bool is_mergeable(double error_rate_before, double error_rate_after, int dim_strings, double alpha);


	/**
	 * Return the number of strings, of the Training Set, not recognized from final automaton.
	 * Note, that is not a generalization index but an error index, because it's calculated on the
	 * training set rather than test set.
	 * @param dfa1
	 * @param positive
	 * @param dim_positive
	 * @param negative
	 * @param dim_negative
	 * @return
	 */
	static double error_rate(RedBlueDfa* dfa1, vector<string>* positive, int dim_positive, vector<string>* negative, int dim_negative, int* &wp, int* &wn, const int tot_wp_w, const int tot_wn_w);

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
	//int  promotion_heuristic_score(RedBlueDfa* dfa1, vector<SYMBOL>* positive, int dim_positive, vector<SYMBOL>* negative, int dim_negative);


public:

	/**
	 * Instance an object with all the members and methods for bluestar inference process.
	 * @param path It's the path where find positive and negative samples
	 * @param alpha
	 * @param delta
	 */
	BlueStar(const char* path, double alpha_value=0.01, double delta_value=0.005);							// Take the path where find samples

	/**
	 * Destroy and bluestar object, freeing the memory.
	 */
	~BlueStar();


	/**
	 * Start an bluestar inference process.
	 * @param path It's the base path where create all the output files of bluestar.
	 * @param exec_time time elapsed, by default is -1, DO NOT pass it as argument if not interested
	 * @return Inferred DFA
	 */
	Dfa* run(string path, double exec_time = -1);											// Argument is the base path where create files


	double get_error_rate_final_dfa();


	static void compute_ir_stats(RedBlueDfa* dfa1, ir_statistical_measures &stats, vector<string>* positive, int dim_positive, vector<string>* negative, int dim_negative, int* &wp, int* &wn);

};


#endif /* BLUESTAR_H_ */
