#include <iostream>
#include "sillyoracle.h"

using namespace std;

SillyOracle::SillyOracle(ConcreteDfa* d1){
	silly_dfa = d1;
};

bool SillyOracle::is_member(vector <string> str){
    return silly_dfa->membership_query(str);
    /*cout << "is " << str << " member?" << endl;

    int x;
    cin >> x;

    if(x==0){
        return false;
    }

    return true;*/
}

bool SillyOracle::is_equiv(ConcreteDfa* dfa_hp , vector <string>* witness_results){
    
    return silly_dfa->equivalence_query(dfa_hp , witness_results);
    /*cout << "is equiv?" << endl;
    string x;
    cin >> x;
    if(x=="0"){
        cout << "why?" << endl;
        while(x!="-1"){

            cin >> x;
            if(x!="-1"){
                witness_results->push_back(x);
            }
        }

    return false;
    }
    
    return true;*/
}
