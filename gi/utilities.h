/*
 * Utility.h
 *
 *  Created on: 29 set 2015
 *      Author: piero
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

//#include "../gilearning/gi_dfa.h"  //NOT INCLUDE NEVER, ELSE CIRCULAR REFERENCE. WE USE FORWARD DECLARATION
//#include <dfaEDSM.h>
#include <iostream>
#include <string>
#include <set>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>    // std::find_if

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#define ND 		numeric_limits<int>::max()
#define MINF 	numeric_limits<int>::min()
#define DMAX 	numeric_limits<double>::max()
#define DMINF 	numeric_limits<double>::min()

namespace mp=boost::multiprecision;

typedef mp::number<mp::backends::cpp_bin_float<3000 ,mp::backends::digit_base_2, void, boost::int32_t> > bigNum;

//-----------------------------------------------------------------------------
//MURMURHASH3 DEFINITIONS
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER) && (_MSC_VER < 1600)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;

// Other compilers

#else	// defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

class Dfa; //FORWARD DECLARATION TO AVOID CIRCULAR DEPENDENCIES

using namespace std;

void MurmurHash3_x86_128 ( const void * key, int len, uint32_t seed, void * out );

void MurmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out );
//-----------------------------------------------------------------------------

// Mi definisco un predicato che uso nella map per cambiare il modo in cui ordina di default
// Infatti in ordine lessicografico 1 viene dopo 01, mentre a me interessa che si tenga conto
//  anche della lunghezza e quindi 1 venga prima di 01
/*struct strsize_less
{
    bool operator()(string const& l, string const& r) const
    {
        if (l.size() < r.size())
            return true;

        if (l.size() > r.size())
            return false;

        return l < r;
    };
};*/



template <typename T>
vector<T> append_vectors(const vector<T> *a,const vector<T> *b){
        vector<T> res = *a;

        res.insert(res.end(), b->begin(), b->end());

        return res;

}

template <typename T, typename Iterator>
vector<T> prefix_vector(vector<T>* a, Iterator pref_end){
	vector<T> res(a->begin(), pref_end);

	return res;
}


template <typename T>
ostream& operator<<(ostream& os, const vector<T> obj)
{
	os<<"[ ";
	for(auto i=obj.begin(); i!=obj.end(); ++i)
		os<< *i <<" ";
	os<<"]";

	return os;
}

string intTostring (int a);

int stringToint(string str);

string charToString(char c);

string trimRight(string str);



////////////////////////////////////////////////////////////////////////////
//  STATISTICAL UTILITIES

/**
 * Given an alpha values for a Normal Gaussian Distribution N(0,1), return the value of U_alpha, that is Z-alpha for N(0,1).
 * It define the bound value for accepting or rejecting an hypothesis, adopting the N(0,1).
 * @param alpha
 * @return  U-alpha value: the z-value for N(0,1), the bound value for accepting and rejecting area.
 */
double u_alpha_score(double alpha);


/**
 * Compute the Z-alpha value for a two proportions test, for two populations, and two samples having the same size. It's a one tailed test.
 * Z-alpha bound value for accepting and rejecting hypothesis, given an alpha value.
 * An upper value of alpha correspond to a bigger area of rejecting state: the process will reject more merge, reducing the Type II error and increasing the Type I error.
 * Type I error means here is: probability to reject a useful merge respect to the error introduced. It's alpha.
 * Type II error means here is: probability to accept a dangerous merge, respecto to the error introduced.
 * If you want an accurate final dfa you would increase the value of alpha. If you want a smaller dfa you wold decrease the value of alpha.
 * @param prop1
 * @param prop2
 * @param sample_size
 * @param alpha
 * @param &dev_std_h0
 * @return Z-alpha value for two proportions test with 2 populations
 */
double z_alpha_from_u_alpha_two_proportions_test(double prop1, double prop2, int sample_size, double alpha, double* dev_std_h0);


/**
 * Doesn't have the dev. std. as parameter. See the other "z_alpha_from_u_alpha_two_proportions_test" function.
 * @param prop1
 * @param prop2
 * @param sample_size
 * @param alpha
 * @return Z-alpha value for two proportions test with 2 populations
 */
double z_alpha_from_u_alpha_two_proportions_test(double prop1, double prop2, int sample_size, double alpha);


bool approximation_conditions_binomial_to_gaussian_distr(double p_est, double q_est, int sample_size);


// Information  retrieval measures
struct ir_statistical_measures
{
	// True positive, and so on.
	int tp = 0;
	int tn = 0;
	int fp = 0;
	int fn = 0;

    int dim_positive=0; //for the target dfa
    int dim_negative=0;  //for the target dfa

    bigNum accuracy  = 0; //(number in [0 1])
	bigNum precision = 0;
	bigNum recall = 0; //sensitivity
	bigNum f_measure = 0;
	bigNum matthews = 0;
	bigNum specificity = 0; //this is recall- or true negative rate
	bigNum bcr = 0;
};


void compute_ir_stats(const Dfa* dfa1 ,const Dfa* target,const vector<string> &test_set,ir_statistical_measures &stats);
void print_ir_stats(ir_statistical_measures &stats);
//void compute_ir_stats(dfaEDSM* dfa1, ir_statistical_measures &stats, vector<SYMBOL>* positive, int dim_positive, vector<SYMBOL>* negative, int dim_negative, int* &wp, int* &wn);



/////////////////////////////////////////////////////////////////////////////


string intTostring(int a);

int stringToint(string str);

double stringTodouble( const string& str );

string doubleTostring(double a);

string charToString(char c);

void shuffleArray(int* array,int size);

int* kfold( int size, int k );

int getPoisson(double lambda);


double ncd(double comp_x, double comp_y, double comp_xy);


// trim from start
inline string &ltrim(string &s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), not1(std::ptr_fun<int, int>(isspace))));
        return s;
}

// trim from end
inline string &rtrim(string &s) {
        s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
        return s;
}

// trim from both ends
inline string &trim(string &s) {
        return ltrim(rtrim(s));
}

//compare function to coparison between hash values in gi_approximateOracle.cpp
struct lex_compare {
    bool operator() (const uint8_t *hash1, const uint8_t *hash2) const {
        return (memcmp(hash2,hash1,16) > 0); //compare 16 byte.
        }
    };




//code to generate permutations with ripetitions
//The length of permutation is length_permutation
//If you pass in second parameter a vector<string> alph={"a","bbb","c"} there will be 3 symbols and the symbols of permutation are in {a,bbb,c}
//This have to be a container: a vector, a map ecc.
template <typename Container>
void permutation_with_ripetitions(const int length_permutation , const Container & c , vector<Container> &results){
	int temp;
	vector<int> current_permutation(length_permutation+1);
	results.resize(pow(c.size(),length_permutation)); //this can be a vector<vector<>> and will contain all the permutations

	int upto = c.size()-1, temp2,num_permutation=0;

	for( temp2 = 1 ; temp2 <= length_permutation; temp2++)
		current_permutation[temp2]=0;

	current_permutation[length_permutation]=-1; //indispensable to handle the first generated permutation
	temp=length_permutation;

	while(1){

		if(current_permutation[temp]==upto){
			temp--;
			if(temp==0)
				break;
		}
		else{
			current_permutation[temp]++;
			while(temp<length_permutation){
				temp++;
				current_permutation[temp]=0;
			}

                        /* PRINT THE PERMUTATIONS
			cout<<"(";
			for( temp2 = 1 ; temp2 <= length_permutation; temp2++){
			      cout<<c[current_permutation[temp2]];
			}
			cout<<")";
			*/
			for( temp2 = 1 ; temp2 <= length_permutation; temp2++)
			      results[num_permutation].push_back(c[current_permutation[temp2]]);

			num_permutation++;

		}
	}

}

template <typename Container1>
void print_permutations(const Container1 & containerToPrint)
{
   for(auto i=containerToPrint.begin() ; i!=containerToPrint.end() ; i++)
   {
      cout<<"(";
      for(auto j=i->begin() ; j!= i->end() ; j++)
         cout<<(*j);
      cout<<")"<<endl;
   }
}

#endif /* UTILITIES_H_ */
