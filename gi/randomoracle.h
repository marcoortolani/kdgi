/*
 * randomoracle.h
 *
 */

#ifndef RANDOMORACLE_H_
#define RANDOMORACLE_H_

#include <random>
#include <math.h>

class RandomOracle{
private:
	map<vector<string>, bool> samples_;		/*!< The pseudo-random set of phrases of this oracle */

public:
	/**
	 * Constructor, it generates the sample set, by choosing a pseudo-random boolean value for every phrase of alphabet*
	 * equal in size to or shorter of  the max length specified.
	 * @param max_length	The max length our samples can reach.
	 * @alphabet			The language alphabet.
	 */
	RandomOracle(int max_length, vector<symbol_> alphabet);

	/**
	 * Every class that will be used ad an oracle must implement this very method.
	 * It returns the acceptance of a phrase.
	 * @param phrase	The phrase to verify.
	 * @return			Whether or not the phrase belongs to the target language.
	 */
	bool membership_query(vector <symbol_> phrase);

	/**
	 * Every class that will be used ad an oracle must implement this very method.
	 * It returns the correctness of the hypothesis and a counterexample if the hypothesis is wrong.
	 * @param dfa_hp			A pointer to the dfa hypothesis.
	 * @param counterexample	Used to pass a counterexample to the hypothesis if it is wrong.
	 * @return					Whether the hypothesis is correct or not.
	 */
	template <class Dfa>
	bool equivalence_query(Dfa* dfa_hp , vector<symbol_>& counterexample);

	/**
	 * Used to debug purposes, it prints all the samples generated with their acceptance.
	 */
	void print();
};

#include "randomoracle.tcc"
#endif /* RANDOMORACLE_H_ */
