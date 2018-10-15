/*
 * randomoracle.h
 *
 *  Created on: 26 set 2018
 *      Author: marco
 */

#ifndef RANDOMORACLE_H_
#define RANDOMORACLE_H_

//#include "oracle.h"
#include <random>
#include <math.h>

class RandomOracle{
private:
	map<vector<string>, bool> samples_;

public:
	RandomOracle(int max_length, vector<symbol_> alphabet);

	virtual bool membership_query(vector <string> str);

	template <class Dfa>
	bool equivalence_query(Dfa* dfa_hp , vector<symbol_>& witness_results);
	//virtual bool equivalence_query(void* dfa_hp , vector<string>& witness_results);

	void print();
};

#include "randomoracle.tcc"
#endif /* RANDOMORACLE_H_ */
