/*
 * Utility.h
 *
 *  Created on: 29 set 2015
 *      Author: piero
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#define INF 0x7fffffff

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>    // std::find_if
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <boost/multiprecision/cpp_bin_float.hpp>
#include <boost/multiprecision/number.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#define ND 		numeric_limits<int>::max()
#define MINF 	numeric_limits<int>::min()
#define DMAX 	numeric_limits<double>::max()
#define DMINF 	numeric_limits<double>::min()
#define INF_UTIL 0x7fffffff

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

class ConcreteDfa; //FORWARD DECLARATION TO AVOID CIRCULAR DEPENDENCIES

//****** In sostitution of fpu_control.h which is Linux specific
/* masking of interrupts */
#define _FPU_MASK_IM  0x01
#define _FPU_MASK_DM  0x02
#define _FPU_MASK_ZM  0x04
#define _FPU_MASK_OM  0x08
#define _FPU_MASK_UM  0x10
#define _FPU_MASK_PM  0x20

/* precision control */
#define _FPU_EXTENDED 0x300	/* libm requires double extended precision.  */
#define _FPU_DOUBLE   0x200
#define _FPU_SINGLE   0x0

/* rounding control */
#define _FPU_RC_NEAREST 0x0    /* RECOMMENDED */
#define _FPU_RC_DOWN    0x400
#define _FPU_RC_UP      0x800
#define _FPU_RC_ZERO    0xC00

#define _FPU_RESERVED 0xF0C0  /* Reserved bits in cw */


/* The fdlibm code requires strict IEEE double precision arithmetic,
   and no interrupts for exceptions, rounding to nearest.  */

#define _FPU_DEFAULT  0x037f

/* IEEE:  same as above.  */
#define _FPU_IEEE     0x037f

/* Type of the control word.  */
typedef unsigned int fpu_control_t __attribute__ ((__mode__ (__HI__)));

/* Macros for accessing the hardware control word.  */
#define _FPU_GETCW(cw) __asm__ ("fnstcw %0" : "=m" (*&cw))
#define _FPU_SETCW(cw) __asm__ ("fldcw %0" : : "m" (*&cw))

/* Default control word set at startup.  */
extern fpu_control_t __fpu_control;

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


void compute_ir_stats(const ConcreteDfa* dfa1 ,const ConcreteDfa* target,const vector<vector<string>> &test_set,ir_statistical_measures &stats);
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


static inline int hungarian(long **mtx, int m, int n, long *solution, long heur)
{
    long *col_mate;
    long *row_mate;
    long *parent_row;
    long *unchosen_row;
    long t;
    long q;
    long *row_dec;
    long *col_inc;
    long *slack;
    long *slack_row;
    long unmatched;

    long *tmtx;
    long transposed;

    long k;
    long l;
    long j;
    long s;


    assert(*mtx);
    assert(m);
    assert(n);

    if (m != n)
	heur = 0;

    if (m > n) {
	tmtx = (long *) malloc(m * n * sizeof(long));
	if (tmtx == NULL) {
	    fprintf(stderr, "Hungarian algorithm: Sorry, out of memory!\n");
	    exit(-1);
	}
	for (k = 0; k < m; k++)
	    for (l = 0; l < n; l++)
		*(tmtx + l * m + k) = *(*mtx + k * n + l);
	m = n;
	n = k;
	free(*mtx);
	*mtx = tmtx;
	transposed = 1;
    } else
	transposed = 0;



    col_mate = (long *) malloc(m * sizeof(long));
    row_mate = (long *) malloc(n * sizeof(long));
    parent_row = (long *) malloc(n * sizeof(long));
    unchosen_row = (long *) malloc(m * sizeof(long));
    row_dec = (long *) malloc(m * sizeof(long));
    col_inc = (long *) malloc(n * sizeof(long));
    slack = (long *) malloc(n * sizeof(long));
    slack_row = (long *) malloc(n * sizeof(long));

    if (!col_mate || !row_mate || !parent_row || !unchosen_row || !row_dec
	|| !col_inc || !slack || !slack_row) {
	fprintf(stderr, "Hungarian algorithm: Sorry, out of memory!\n");
	exit(-1);
    }

    if (heur) {
	for (l = 0; l < n; l++) {
	     s = (*mtx)[0*n+l];
	    for (k = 1; k < n; k++)
		if ( (*mtx)[k*n+l] < s)
		    s = (*mtx)[k*n+l];
	    if (s != 0)
		for (k = 0; k < n; k++)
		     (*mtx)[k*n+l] -= s;
	}
    }

    t = 0;
    for (l = 0; l < n; l++) {
	 row_mate[l] = -1;
	 parent_row[l] = -1;
	 col_inc[l] = 0;
	 slack[l] = INF;
    }
    for (k = 0; k < m; k++) {
	 s = (*mtx)[k*n+0];
	for (l = 1; l < n; l++)
	    if ( (*mtx)[k*n+l] < s)
		s = (*mtx)[k*n+l];
	 row_dec[k] = s;
	for (l = 0; l < n; l++)
	    if (( s == (*mtx)[k*n+l]) && ( row_mate[l] < 0)) {
		 col_mate[k] = l;
		 row_mate[l] = k;
		goto row_done;
	    }
	 col_mate[k] = -1;
	 unchosen_row[t++] = k;
      row_done:;
    }

    if (t == 0)
	goto done;
    unmatched = t;
    while (1) {
	q = 0;
	while (1) {
	    while (q < t) {


		{
		     k = unchosen_row[q];
		     s = row_dec[k];
		    for (l = 0; l < n; l++)
			if ( slack[l]) {
			    long del;
			     del = (*mtx)[k*n+l] - s + col_inc[l];
			    if (del < slack[l]) {
				if (del == 0) {
				    if ( row_mate[l] < 0)
					goto breakthru;
				     slack[l] = 0;
				     parent_row[l] = k;
				     unchosen_row[t++] = row_mate[l];
				} else {
				     slack[l] = del;
				     slack_row[l] = k;
				}
			    }
			}
		}

		q++;
	    }


	    s = INF;
	    for (l = 0; l < n; l++)
		if ( slack[l] && slack[l] < s)
		    s = slack[l];
	    for (q = 0; q < t; q++)
		 row_dec[unchosen_row[q]] += s;
	    for (l = 0; l < n; l++)
		if ( slack[l]) {
		     slack[l] -= s;
		    if (slack[l] == 0) {
			 k = slack_row[l];
			if ( row_mate[l] < 0) {
			    for (j = l + 1; j < n; j++)
				if ( slack[j] == 0)
				     col_inc[j] += s;
			    goto breakthru;
			} else {
			     parent_row[l] = k;
			     unchosen_row[t++] = row_mate[l];
			}
		    }

		} else
		     col_inc[l] += s;

	}
      breakthru:

	while (1) {
	     j = col_mate[k];
	     col_mate[k] = l;
	     row_mate[l] = k;
	    if (j < 0)
		break;
	     k = parent_row[j];
	    l = j;
	}

	if (--unmatched == 0)
	    goto done;


	t = 0;
	for (l = 0; l < n; l++) {
	     parent_row[l] = -1;
	     slack[l] = INF;
	}
	for (k = 0; k < m; k++)
	    if ( col_mate[k] < 0) {
		 unchosen_row[t++] = k;
	    }

    }
  done:

    for (k = 0; k < m; k++)
	for (l = 0; l < n; l++)
	    if ((*mtx)[k*n+l] < row_dec[k] - col_inc[l]) {
		printf("%d %d\n",m,n);
		for(int o=0; o<m; o++)
		{
		    for(int u=0; u<n; u++)
			printf("%ld ",(*mtx)[o*n+u]);
		    putchar('\n');
		}
		putchar('\n');
		fprintf(stderr, "Hungarian algorithm: Oops, I made a mistake!\n");
		exit(-1);
		return -6;
	    }
    for (k = 0; k < m; k++) {
	l = col_mate[k];
	if (l < 0 || (*mtx)[k*n+l] != row_dec[k] - col_inc[l]) {
	    fprintf(stderr, "Hungarian algorithm: Oops, I blew it!\n");
	    exit(-1);
	    return -66;
	}
    }
    k = 0;
    for (l = 0; l < n; l++)
	if (col_inc[l])
	    k++;
    if (k > m) {
	fprintf(stderr, "Hungarian algorithm: Oops, I adjusted too many columns!\n");
	exit(-1);
	return -666;
    }

    if (!transposed)
	memcpy(solution, col_mate, m * sizeof(long));
    else
    {
	for (k = 0; k < n; k++)
	    solution[k]=-1;
	for (k = 0; k < m; k++)
	    solution[col_mate[k]] = k;
    }

    free(col_mate);
    free(row_mate);
    free(parent_row);
    free(unchosen_row);
    free(row_dec);
    free(col_inc);
    free(slack);
    free(slack_row);

    return 0;
}


class DfaSim {

  private:

    const ConcreteDfa* dfa_reference_;
    const ConcreteDfa* dfa_subject_;

    /****** W-METHOD STATS ******/
    long double true_positives_;
    long double true_negatives_;
    long double false_positives_;
    long double false_negatives_;
    long double precision_;
    long double recall_;
    long double linguistical_f_measure_;
    long double specificity_;
    long double bcr_;

    /****** NEIGHBOUR MATCHING STATS ******/
    vector<vector<double>> nodes_sim_matrix_;
    double structural_f_measure_;

	long double exec_time_;
  
  public:

	DfaSim();

    DfaSim(const ConcreteDfa* reference, const ConcreteDfa* subject, vector<long double> &w_method_stats, vector<vector<double>> &neighbour_matching_stats);
    
  /**
   * Print the similarity score between dfas, taking into account both linguistical
   * and structural sides.
   */
  void print_sim()const;

  /**
   * Print the ttable of reference and subject dfas
   */
  void which_dfas()const;

  /**
  * Returns the two dfas compared
  * @param subject here the function returns the dfa_subject_
  * @return dfa_reference_
  */
  const ConcreteDfa* which_dfas(const ConcreteDfa* subject) const;

  long double get_true_positives() const;

  long double get_true_negatives() const;

  long double get_false_positives() const;

  long double get_false_negatives() const;

  long double get_precision() const;

  long double get_recall() const;

  long double get_linguistical_f_measure() const;

  long double get_specificity() const;

  long double get_bcr() const;

  vector<vector<double>> get_nodes_sim_matrix() const;

  double get_structural_f_measure() const;

  long double get_exec_time() const;

};

#endif /* UTILITIES_H_ */
