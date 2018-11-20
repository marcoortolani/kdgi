#ifndef SILLY_ORACLE_H
#define SILLY_ORACLE_H

#include <iostream>
#include "concretedfa.h"

using namespace std;

class SillyOracle{
private:
	vector<vector<symbol_>> dataset_;
public:
	SillyOracle(vector<vector<symbol_>> ds);

	bool membership_query(vector <string> str);

	template <class Dfa>
	bool equivalence_query(Dfa* dfa_hp , vector<symbol_>& witness_results);
};

#include "sillyoracle.tcc"

#endif
