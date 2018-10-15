#ifndef LEARNER_H
#define LEARNER_H

//#include "oracle.h"
#include <tuple>

template <class Iter, class Oracle>
class Learner{
    protected:
        Oracle* oracle;
        vector<symbol_> alphabet;

        int equivalence_cost;
        int membership_cost;
        int symbol_cost;
        int repetition_cost;
        map<vector<symbol_>, bool> asked_queries;

        bool ask_membership(vector<symbol_> phrase);
        bool ask_equivalence(Dfa<Iter>* hypotesis, vector<symbol_>&counterexample);

    public:
        //Learner();
        tuple<int, int, int, int> get_costs();
        void reset_costs();

        Learner(Oracle* o, vector <string> ab);

        void set_oracle(Oracle* o);

        void set_alphabet(vector <string> ab);

        virtual Dfa<Iter>* learn()=0;
};

#include "learner.tcc"
#endif
