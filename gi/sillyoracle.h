#ifndef SILLY_ORACLE_H
#define SILLY_ORACLE_H

#include "oracle.h"
#include "concretedfa.h"

class SillyOracle : public Oracle{
private:
	ConcreteDfa* silly_dfa;//Must be "concrete" for now, will be "general"

public:
	SillyOracle(ConcreteDfa* d1);

	virtual bool is_member(vector <string> str);

	virtual bool is_equiv(ConcreteDfa* dfa_hp , vector <string>* witness_results);
};

#endif
