#include "../../gi/dfa.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main(){
    Dfa* reference = nullptr;
    reference= new Dfa(reference->read_dfa_file("../data/tomita15.txt"));
    reference->print_dfa_ttable("reference");
    Dfa* test= new Dfa();
    *test=*reference;
    test->print_dfa_ttable("test");
    Dfa test2 =  Dfa(reference->get_num_states(),reference->get_alphabet(),reference->get_start_state(),reference->get_ttable(),reference->get_accepting_states());
    test2.print_dfa_ttable("test2");

    return 0;
}