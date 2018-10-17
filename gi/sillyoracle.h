#ifndef SILLY_ORACLE_H
#define SILLY_ORACLE_H

#include <iostream>
//#include "oracle.h"
#include "concretedfa.h"

using namespace std;

class SillyOracle{
private:
	ConcreteDfa* silly_dfa;//Must be "concrete" for now, will be "general"

public:
	SillyOracle(ConcreteDfa* d1);

	bool membership_query(vector <string> str);

	template <class Dfa>
	bool equivalence_query(Dfa* dfa_hp , vector<symbol_>& witness_results);
};

#include "sillyoracle.tcc"

#endif
