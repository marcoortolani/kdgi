#ifndef LEARNER_H
#define LEARNER_H

#include "oracle.h"

class Learner{
    protected:
        Oracle* oracle;
        vector <string> alphabet;

    public:

        Learner();
        
        Learner(Oracle* o, vector <string> ab);

        void set_oracle(Oracle* o);

        void set_alphabet(vector <string> ab);

        virtual Dfa* learn()=0;
};
#endif
