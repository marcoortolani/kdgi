#ifndef ORACLE_H
#define ORACLE_H

#include "concretedfa.h"
#include <string>

class Oracle{
    public:
        virtual bool is_member(vector<string> str) = 0;

        virtual bool is_equiv(ConcreteDfa* dfa_hp , vector<string>* witness_results) = 0; //Must be concrete for now, will be "general"
};

#endif
