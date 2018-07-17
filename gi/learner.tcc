/*
 * learner.tcc
 */

template <class ds>
Learner<ds>::Learner(){
    
    oracle = NULL;
    alphabet = vector <string>();
}

template <class ds>
Learner<ds>::Learner(Oracle* o, vector <string> ab){
	
    set_oracle(o);
    set_alphabet(ab);
}

template <class ds>
void Learner<ds>::set_oracle(Oracle* o){
    oracle = o;
}

template <class ds>
void Learner<ds>::set_alphabet(vector<symbol_> ab){
    alphabet = ab;
}