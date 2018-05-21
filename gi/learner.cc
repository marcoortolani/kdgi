#include "learner.h"

Learner::Learner(){
    
    oracle = NULL;
    alphabet = vector <string>();
}

Learner::Learner(Oracle* o, vector <string> ab){

    set_oracle(o);
    set_alphabet(ab);
}

void Learner::set_oracle(Oracle* o){
    oracle = o;
}

void Learner::set_alphabet(vector <string> ab){
    alphabet = ab;
}

