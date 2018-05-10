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
#define INF 0x7fffffff

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

//Useful for neighbour matching similarity
class Graph
{
private:
    vector<int> labels;
    vector<int> out_edges_count;
    vector<int> edges;
    vector<vector<int> *> nodes;
    void LoadGraph(FILE *f)
    {
		int n1, n2, nc, nm;
			fscanf(f,"%u", &nc);
		for(int i=0; i<nc; i++)
		{
			fscanf(f,"%d",&nm);
			labels.push_back(nm);
			nodes.push_back(new vector<int>);
			out_edges_count.push_back(0);
		}
		while(1)
		{
			fscanf(f,"%d %d",&n1,&n2);
			if(feof(f))
			break;
			assert(0<=n1 && n1<nc);
			assert(0<=n2 && n2<nc);
			AddEdge(n1,n2);
		}
    }
    void LoadGraph(char *matrix, int n, vector<int> *l)
    {
		labels.clear();
		for(int i=0; i<n; i++)
			labels.push_back((*l)[i]);
		for(int i=0; i<n; i++)
		{
			nodes.push_back(new vector<int>);
			out_edges_count.push_back(0);
		}
		for(int i=0; i<n; i++)
			for(int j=0; j<n; j++)
				if(matrix[i*n+j])
					AddEdge(i,j);
    }
    void AddEdge(int n1, int n2)
    {
		edges.push_back(n1);
		edges.push_back(n2);
		if(nodes[n1]->size()==0)
			nodes[n1]->push_back(edges.size()/2-1);
		else
			nodes[n1]->insert(nodes[n1]->begin(),edges.size()/2-1);
		out_edges_count[n1]++;
		nodes[n2]->push_back(edges.size()/2-1);
    }
    void Clear()
    {
		for(unsigned i=0; i<nodes.size(); i++)
			delete nodes[i];
		nodes.clear();
		out_edges_count.clear();
		edges.clear();
		labels.clear();
    }
public:
    Graph(string file_name)
    {
		Reload(file_name);
    }
    Graph(char *matrix, int n, vector<int> *l)
    {
		Reload(matrix, n, l);
    }
    ~Graph()
    {
		for(unsigned i=0; i<nodes.size(); i++)
	    	delete nodes[i];
    }
    void Reload(string file_name)
    {
		FILE *f;
		Clear();
		if((f=fopen(file_name.c_str(),"r"))==0)
		{
			printf("File %s can't be opened!\n",file_name.c_str());
			throw "";
		}
		LoadGraph(f);
		fclose(f);
    }
    void Reload(char *matrix, int n, vector<int> *l)
    {
		Clear();
		if(!matrix)
		{
			printf("Invalid matrix!\n");
			throw "";
		}
		LoadGraph(matrix, n, l);
    }
    int NodeCount()
    {
		return nodes.size();
    }
    int EdgeCount()
    {
		return edges.size()/2;
    }
    int SourceNode(int edge_no)
    {
		return edges[2*edge_no];
    }
    int TerminatingNode(int edge_no)
    {
		return edges[2*edge_no+1];
    }
    void EnumBeginningEdges(int node, vector<int>::iterator *array, int *length)
    {
		if(array)
			*array=nodes[node]->begin();
		if(length)
			*length=BeginningEdgesCount(node);
    }
    void EnumTerminatingEdges(int node, vector<int>::iterator *array, int *length)
    {
		if(array)
			*array=nodes[node]->begin()+out_edges_count[node];
		if(length)
			*length=TerminatingEdgesCount(node);
    }
    int BeginningEdgesCount(int node)
    {
		return out_edges_count[node];
    }
    int TerminatingEdgesCount(int node)
    {
		return nodes[node]->size()-out_edges_count[node];
    }
    int Label(int i)
    {
		return labels[i];
    }
};

class NMSimilarity
{
private:
    double **node_similarity;
    double **tmp_similarity;
    Graph *graph_a;
    Graph *graph_b;
    int graph_a_n;
    int graph_b_n;
    double epsilon;
    long *solution;
    long *costs;
    int first_time;
    
    int Iteration(double eps)
    {
		int precision_achieved=1;
		for(int i=0; i<graph_a_n; i++)
			for(int j=0; j<graph_b_n; j++)
			tmp_similarity[i][j]=node_similarity[i][j];
		for(int i=0; i<graph_a_n; i++)
			for(int j=0; j<graph_b_n; j++)
			{
			if(graph_a->Label(i)!=graph_b->Label(j))
				continue;
			vector<int>::iterator kaddr,laddr;
			int klen, llen;
			double in_similarity=0;
			double out_similarity=0;
			graph_a->EnumBeginningEdges(i,&kaddr,&klen);
			graph_b->EnumBeginningEdges(j,&laddr,&llen);
				if(klen>0 && llen>0)
			{
				costs=(long *)malloc(klen*llen*sizeof(long));
				solution=new long[klen];
				for(int k=0; k<klen; k++)
					for(int l=0; l<llen; l++)
						costs[k*llen+l]=(1-tmp_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[l])])/eps;
				
				hungarian(&costs, klen, llen, solution, 0);
				for(int k=0; k<klen; k++)
				if(solution[k]>=0)
					out_similarity+=tmp_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[solution[k]])];
				delete solution;
				free(costs);
			}
			if(max(klen,llen)!=0)
			out_similarity/=(max(klen,llen));
			else
			out_similarity=1;

			graph_a->EnumTerminatingEdges(i,&kaddr,&klen);
			graph_b->EnumTerminatingEdges(j,&laddr,&llen);
			if(klen>0 && llen>0)
			{
				costs=(long *)malloc(klen*llen*sizeof(long));
				solution=new long[klen];
				for(int k=0; k<klen; k++)
					for(int l=0; l<llen; l++)
						costs[k*llen+l]=(1-tmp_similarity[graph_a->SourceNode(kaddr[k])][graph_b->SourceNode(laddr[l])])/eps;
				hungarian(&costs, klen, llen, solution, 0);			
				for(int k=0; k<klen; k++)
				if(solution[k]>=0)
					in_similarity+=tmp_similarity[graph_a->SourceNode(kaddr[k])][graph_b->SourceNode(laddr[solution[k]])];
				delete solution;
				free(costs);
			}
			if(max(klen,llen)!=0)
			in_similarity/=(max(klen,llen));
			else
			in_similarity=1;

			node_similarity[i][j]=(in_similarity+out_similarity)/2;
			if(fabs(tmp_similarity[i][j]-node_similarity[i][j])>=eps)
				precision_achieved=0;
			}
		return precision_achieved;
    }
    void Clear()
    {
		for(int i=0; i<graph_a_n; i++)
			delete node_similarity[i];
		delete node_similarity;
		for(int i=0; i<graph_a_n; i++)
			delete tmp_similarity[i];
		delete tmp_similarity;
    }
    
public:
    NMSimilarity(Graph *gr_a, Graph *gr_b): graph_a(gr_a), graph_b(gr_b)
    {
		first_time=1;
		Reload(gr_a,gr_b);
		first_time=0;
    }
    void Reload(Graph *gr_a, Graph *gr_b)
    {
		if(!first_time)
			Clear();
		graph_a=gr_a;
		graph_b=gr_b;
		graph_a_n=graph_a->NodeCount();
		graph_b_n=graph_b->NodeCount();
		node_similarity=new double*[graph_a_n];
		tmp_similarity=new double*[graph_a_n];
		for(int i=0; i<graph_a_n; i++)
		{
			node_similarity[i]=new double[graph_b_n];
			for(int j=0; j<graph_b_n; j++)
			node_similarity[i][j]=graph_a->Label(i)==graph_b->Label(j);
			tmp_similarity[i]=new double[graph_b_n];
		}
    }
    ~NMSimilarity()
    {
		Clear();
    }
    int Iterate(double eps, int max_iter)
    {
		int iter=0;
		int it=0;
		while(!it && iter<max_iter)
		{
			it=Iteration(eps);
			iter++;
		}	
		return iter;
    }
    double NodeSimilarity(int i, int j)
    {
		if(i>=graph_a_n || j>=graph_b_n)
			return -1;
		return node_similarity[i][j];
    }

static int hungarian(long **mtx, int m, int n, long *solution, long heur)
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

    register long k;
    register long l;
    register long j;
    register long s;
    
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
			    register long del;
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

};

inline char* get_incidence_matrix(int** ttable, int num_states, int dim_alphabet){

	char* incidence_matrix=new char[num_states*num_states];

	for(int i=0;i<(num_states*num_states);++i)
		incidence_matrix[i]=0;

	int arrive_state=0;
	for(int row=0;row<num_states;++row)
		for(int column=0;column<dim_alphabet;++column){
			arrive_state=ttable[row][column];
			incidence_matrix[row*num_states+arrive_state]=1;
		}

	return incidence_matrix;

}

#endif /* UTILITIES_H_ */
