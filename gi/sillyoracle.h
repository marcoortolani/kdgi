#ifndef SILLY_ORACLE_H
#define SILLY_ORACLE_H

#include "oracle.h"
#include "dfa.h"

class SillyOracle : public Oracle{
private:
	Dfa* silly_dfa;

public:
	SillyOracle(Dfa* d1);

	virtual bool is_member(vector <string> str);

	virtual bool is_equiv(Dfa* dfa_hp , vector <string>* witness_results);
};

#endif
