#ifndef ORACLE_H
#define ORACLE_H

#include "dfa.h"
#include <string>

class Oracle{
    public:
        virtual bool is_member(vector<string> str) = 0;

        virtual bool is_equiv(Dfa* dfa_hp , vector<string>* witness_results) = 0;
};

#endif
