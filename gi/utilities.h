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
		for(int i=0; i<n; i++){
			labels.push_back((*l)[i]);
		}
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

	// iterazione corrente
    double **node_similarity;

	// ultima iterazione
    double **tmp_similarity;

	// penultima iterazione -> non serve
	//double **second_to_last_similarity;

	// storia delle iterazioni
	//vector<vector<vector<double> > > history_similarity;

	// ogni coppia di stati, ha un suo vettore di coppie (i,j), i del primo grafo, j del secondo grafo
	// che hanno fatto variare il loro punteggio di similarità all' ultima iterazione
	// differenziando il discorso in entrata e in uscita
	// una tupla è una tripla in cui i primi due parametri sono i nodi effettivi
	// il terzo parametro rappresenta il numero dell'iterazione in cui è stata rilevata la modifica
	// questo ci è utile perchè se la coppia modificante che stiamo considerando all'iterazione corrente
	// è gia stata presa in esame in passato, allora il loro punteggio di similarità da sommare
	// deve essere preso dalla matrice dell'iterazione precedente a quella in cui è stata aggiunta la coppia
	vector<vector<vector<tuple<int, int, double> > > > last_modified_by_in;
	vector<vector<vector<tuple<int, int, double> > > > last_modified_by_out;


    Graph *graph_a;
    Graph *graph_b;
    int graph_a_n;
    int graph_b_n;
    long *solution;
    long *costs;
    int first_time;

    int Iteration(double eps, int iter)
    {
		// flag per fermare l'iterazione, 1 significa che la condizione di terminazione è soddisfatta
		int precision_achieved=1;

		double sim_to_add = 0;

		// accumulatore per la corrente iterazione
		//vector<vector<vector<pair<int, int> > > > temp_last_modified_by_in(graph_a_n, vector<vector<pair<int,int> > >(graph_b_n, vector<pair<int, int> >()));
		//vector<vector<vector<pair<int, int> > > > temp_last_modified_by_out(graph_a_n, vector<vector<pair<int,int> > >(graph_b_n, vector<pair<int, int> >()));
		#ifdef DEBUG
    	std::cout<<std::endl<<"****************************************************"<<std::endl<<"Iterazione: "<<iter<<std::endl<<std::endl;
		#endif
		
		for(int i=0; i<graph_a_n; i++)
			for(int j=0; j<graph_b_n; j++){
				//if(iter>=1)
					// la penultima iterazione è uguale a quella che la precedente iterazione era l'ultima iterazione
					//second_to_last_similarity[i][j]=tmp_similarity[i][j];

				// l'ultima iterazione è uguale a quella corrente prima di effettuare un qualsiasi conto
				tmp_similarity[i][j]=node_similarity[i][j];
			}

		for(int i=0; i<graph_a_n; i++)
			for(int j=0; j<graph_b_n; j++)
			{
				// faccenda stati accettanti/rigettanti
				if(graph_a->Label(i)!=graph_b->Label(j))
					continue;

				// nodi del vicinato: kaddr == vicinato del nodo del grafo A; laddr == vicinato del nodo del grafo B
				vector<int>::iterator kaddr,laddr;

				// cardinalità del vicinato
				int klen, llen;

				//vector<int>::iterator opt_match_kaddr,opt_match_laddr;
				//int opt_match_klen, opt_match_llen;

				double in_similarity=0;
				double out_similarity=0;

				// nodi e cardinalità del vicinato uscente
				graph_a->EnumBeginningEdges(i,&kaddr,&klen);
				graph_b->EnumBeginningEdges(j,&laddr,&llen);

				if(klen>0 && llen>0)
				{
					// costs è la sottomatrice costituita dal vicinato (entrante o uscente)
					// che contiene i pesi, cioe i punteggi di similarità internodo,
					// è in sostanza la matrice su cui risolvere l'assignment problem per
					// trovare il matching ottimo
					costs=(long *)malloc(klen*llen*sizeof(long));
					solution=new long[klen];
					for(int k=0; k<klen; k++)
						for(int l=0; l<llen; l++)
							// l'hungarian algorithm di suo risolve il MINIMUM assignment problem
							// noi siamo interessati al MAXIMUM assignment problem, ecco che
							// prendiamo il complementare rispetto a 1 di ciascun peso.
							costs[k*llen+l]=(1-tmp_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[l])])/eps;

					// solution è un vettore di dimensione pari alla cardinalità del vicinato del nodo
					// del grafo A, e cioè i della corrente iterazione.
					// l'indice con cui si accede a solution indica il nodo del vicinato di i appartenente al grafo A
					// (indice scalato alla sottomatrice che comprende solo il vicinato, kaddr ed laddr consentono di recuperare il nodo vero a partire dall'indice scalato)
					// il valore contenuto nel vettore indica quale nodo del vicinato di j appartenente al grafo B
					// si deve associare all'indice per avere il matching ottimo.
					// un valore negativo indica che il nodo i non fa parte del matching
					hungarian(&costs, klen, llen, solution, 0);

					#ifdef HUNG_DEBUG
						cout<<endl<<"Coppia ("<<i<<","<<j<<"), uscente:"<<endl;
					#endif

					for(int k=0; k<klen; k++){

						#ifdef HUNG_DEBUG
							cout<<"K="<<k<<" solution[k]="<<solution[k]<<endl;
						#endif

						if(solution[k]>=0){

							std::vector<std::tuple<int, int, double> >:: iterator it ;
							bool already_modified = false;

							// Controlla se la coppia del matching ottimo, il cui valore locale di similarità alla precedente iterazione
							// è da sommare al punteggio di similarità dei nodi (i,j) alla iterazione corrente,
							// è stata già tenuta in considerazione nell'ultima iterazione:
							// se è cosi allora pesca il punteggio di similarità della coppia dalla penultima iterazione

							for (it = last_modified_by_out[i][j].begin(); it != last_modified_by_out[i][j].end(); ++it)

								if ((get<0>(*it) == graph_a->TerminatingNode(kaddr[k])) && (get<1>(*it) == graph_b->TerminatingNode(laddr[solution[k]]))){

									sim_to_add = get<2>(*it);

									// fare così, cioe guardare alla penultima iterazione, va bene e.g. per la coppia (0,0) fino alla iterazione 3, perchè poi,
									// nonostante le liste modificanti siano saturate, il punteggio di similarità cambia perchè non tiene memoria di quei calcoli
									// svolti per il vicinato: ad esempio il punteggio di similarita entrante s_in(0,0) = 1/2 * (x_3_3_iterazione_0 + x_1_1_iterazione_1)
									//sim_to_add = second_to_last_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[solution[k]])];

									#ifdef DEBUG2
										if(iter==3){
										cout<<endl<<"history_similarity[get<2>(*it)-1] = "<<get<2>(*it)-1<<endl;	//per la coppia (0,0) alla iterazione 3, quando guarda al vicinato entrante(3,3) dovrebbe restituire l'iterazione a cui guardare, cioè in questo caso 0. E lo stampa, giusto
										cout<<endl<<"Confronto nodi ("<<i<<", "<<j<<"): "<<endl;
										cout<<"La coppia del vicinato uscente ("<<graph_a->TerminatingNode(kaddr[k])<<", "<<graph_b->TerminatingNode(laddr[solution[k]])<<"):"<<endl;
										cout<<"è già stata considerata all'iterazione precedente."<<endl<<"Considero il punteggio della iterazione STORICA precedente a quella in cui la coppia è stata aggiunta: "<<sim_to_add<<endl;
										}
									#endif

									already_modified = true;
									break;
								}

							// se la coppia non è già intervenuta nella passata iterazione, considera
							// il loro punteggio all' ultima iterazione
							if(!already_modified){

								precision_achieved=0;

								sim_to_add = tmp_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[solution[k]])];

								#ifdef DEBUG2
									if(iter==3){
									cout<<endl<<"Confronto nodi ("<<i<<", "<<j<<"): "<<endl;
									cout<<"La coppia del vicinato uscente ("<<graph_a->TerminatingNode(kaddr[k])<<", "<<graph_b->TerminatingNode(laddr[solution[k]])<<"):"<<endl;
									cout<<"NON è stata considerata all'iterazione precedente."<<endl<<"Considero il punteggio della PRECEDENTE iterazione: "<<sim_to_add<<endl<<tmp_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[solution[k]])]<<endl;
									}
								#endif

								// aggiungi all' insieme di coppie modificanti il punteggio di similarità corrente
								// solo se sei alla prima iterazione, o se il punteggio di similarità della coppia in esame
								// è diverso da quello alla penultima iterazione


								//graph_a->EnumBeginningEdges(i,&opt_match_kaddr,&opt_match_klen);
								//graph_b->EnumBeginningEdges(j,&opt_match_laddr,&opt_match_llen);
								// la condizione guarda alle coppie di nodi ritornate dal matching ottimo
								// alla prima iterazione, se queste coppie di nodi hanno al loro interno un numero diverso
								// di transizioni uscenti/entranti, allora sono "modificanti"
								/* if(iter == 0 && opt_match_klen!=opt_match_llen){

									last_modified_by_out[i][j].push_back(std::make_pair(graph_a->TerminatingNode(kaddr[k]), graph_b->TerminatingNode(laddr[solution[k]])));

									#ifdef DEBUG0
										cout<<"Sei all'iterazione 0. Aggiungi per la coppia ("<<i<<","<<j<<") la coppia modificante uscente: ("<<graph_a->TerminatingNode(kaddr[k])<<", "<<graph_b->TerminatingNode(laddr[solution[k]])<<")"<<endl;
									#endif

								} */

								//if(iter >= 1 && tmp_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[solution[k]])] != second_to_last_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[solution[k]])]){
								if(iter >=1 && tmp_similarity[graph_a->TerminatingNode(kaddr[k])][graph_b->TerminatingNode(laddr[solution[k]])] < 1){
									last_modified_by_out[i][j].push_back(std::make_tuple(graph_a->TerminatingNode(kaddr[k]), graph_b->TerminatingNode(laddr[solution[k]]), sim_to_add));

									#ifdef DEBUG2
										if(iter==3){
										cout<<endl<<"La coppia del vicinato uscente: ("<<graph_a->TerminatingNode(kaddr[k])<<", "<<graph_b->TerminatingNode(laddr[solution[k]])<<")"<<endl;
										//cout<<"ha un punteggio di similarità che è cambiato tra l'ultima iterazione e la penultima. Dunque aggiungila"<<endl;
										cout<<"ha un punteggio di similarità < 1. Dunque aggiungila"<<endl;
										cout<<"al vettore delle coppie modificanti per lo stato "<<i<<endl;
										}
									#endif

								}
							}

							out_similarity+=sim_to_add;

							#ifdef DEBUG0
								if(iter==0){
									cout<<"Confronto nodo "<<i<<" del grafo A con il nodo "<<j<<" del grafo B"<<endl;
									cout<<"Punteggio sim uscente prima di dividere per il max degli archi uscenti: "<<out_similarity<<endl;
								}
							#endif
						}
					}
					delete solution;
					free(costs);
				}

				if(max(klen,llen)!=0)
					out_similarity/=(max(klen,llen));
				else
					out_similarity=1;



				/* Fine similarità uscente */



				// nodi e cardinalità del vicinato entrante
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

					#ifdef HUNG_DEBUG
							cout<<endl<<"Coppia ("<<i<<","<<j<<"), entrante:"<<endl;
					#endif

					for(int k=0; k<klen; k++){

						#ifdef HUNG_DEBUG
							cout<<"K="<<k<<" solution[k]="<<solution[k]<<endl;
						#endif

						if(solution[k]>=0){

							std::vector<std::tuple<int, int, double> >:: iterator it ;
							bool already_modified = false;

							// Controlla se la coppia del matching ottimo da sommare al punteggio di similarità
							// è stata già tenuta in considerazione nell'ultima iterazione:
							// se è cosi allora pesca il punteggio di similarità della coppia dalla penultima iterazione

							for (it = last_modified_by_in[i][j].begin(); it != last_modified_by_in[i][j].end(); ++it)

								if (get<0>(*it) == graph_a->SourceNode(kaddr[k]) && get<1>(*it) == graph_b->SourceNode(laddr[solution[k]])){

									sim_to_add = get<2>(*it);

									// fare così, cioe guardare alla penultima iterazione, va bene e.g. per la coppia (0,0) fino alla iterazione 3, perchè poi,
									// nonostante le liste modificanti siano saturate, il punteggio di similarità cambia perchè non tiene memoria di quei calcoli
									// svolti per il vicinato: ad esempio il punteggio di similarita entrante s_in(0,0) = 1/2 * (x_3_3_iterazione_0 + x_1_1_iterazione_1)
									//sim_to_add = second_to_last_similarity[graph_a->SourceNode(kaddr[k])][graph_b->SourceNode(laddr[solution[k]])];

									#ifdef DEBUG2
										if(iter==3){
										cout<<endl<<"Confronto nodi ("<<i<<", "<<j<<"): "<<endl;
										cout<<"La coppia del vicinato entrante ("<<graph_a->SourceNode(kaddr[k])<<", "<<graph_b->SourceNode(laddr[solution[k]])<<"):"<<endl;
										cout<<"è già stata considerata all'iterazione precedente."<<endl<<"Considero il punteggio della iterazione STORICA precedente a quella in cui la coppia è stata aggiunta: "<<sim_to_add<<endl;
										}
									#endif

									already_modified = true;
									break;
								}

							// se la coppia non è già intervenuta nella passata iterazione, considera proprio
							// il loro punteggio alla passata iterazione

							if(!already_modified){

								precision_achieved=0;

								sim_to_add = tmp_similarity[graph_a->SourceNode(kaddr[k])][graph_b->SourceNode(laddr[solution[k]])];

								#ifdef DEBUG2
									if(iter==3){
									cout<<endl<<"Confronto nodi ("<<i<<", "<<j<<"): "<<endl;
									cout<<"La coppia del vicinato entrante ("<<graph_a->SourceNode(kaddr[k])<<", "<<graph_b->SourceNode(laddr[solution[k]])<<"):"<<endl;
									cout<<"NON è stata considerata all'iterazione precedente."<<endl<<"Considero il punteggio della PRECEDENTE iterazione: "<<sim_to_add<<endl<<tmp_similarity[graph_a->SourceNode(kaddr[k])][graph_b->SourceNode(laddr[solution[k]])]<<endl;
									}
								#endif

								// aggiungi all' insieme di coppie modificanti il punteggio di similarità corrente
								// solo se o sei alla prima iterazione, o se il punteggio di similarità della coppia in esame
								// è diverso da quello alla penultima iterazione

								// la condizione guarda alle coppie di nodi ritornate dal matching ottimo
								// alla prima iterazione, se queste coppie di nodi hanno al loro interno un numero diverso
								// di transizioni uscenti/entranti, allora sono "modificanti"

								//graph_a->EnumTerminatingEdges(i,&opt_match_kaddr,&opt_match_klen);
								//graph_b->EnumTerminatingEdges(j,&opt_match_laddr,&opt_match_llen);


								/* if(iter == 0 && opt_match_klen!=opt_match_llen){

									last_modified_by_in[i][j].push_back(std::make_pair(graph_a->SourceNode(kaddr[k]), graph_b->SourceNode(laddr[solution[k]])));

									#ifdef DEBUG0
										cout<<"Sei all'iterazione 0. Aggiungi per la coppia ("<<i<<","<<j<<") la coppia modificante entrante: ("<<graph_a->SourceNode(kaddr[k])<<", "<<graph_b->SourceNode(laddr[solution[k]])<<")"<<endl;
									#endif
								} */

								//if(iter >= 1 && second_to_last_similarity[graph_a->SourceNode(kaddr[k])][graph_b->SourceNode(laddr[solution[k]])] != tmp_similarity[graph_a->SourceNode(kaddr[k])][graph_b->SourceNode(laddr[solution[k]])]){
								if(iter >= 1 && tmp_similarity[graph_a->SourceNode(kaddr[k])][graph_b->SourceNode(laddr[solution[k]])]<1){
									#ifdef DEBUG2
										if(iter==3){
										cout<<endl<<"La coppia del vicinato entrante: ("<<graph_a->SourceNode(kaddr[k])<<", "<<graph_b->SourceNode(laddr[solution[k]])<<")"<<endl;
										//cout<<"ha un punteggio di similarità che è cambiato tra l'ultima iterazione e la penultima. Dunque aggiungila"<<endl;
										cout<<"ha un punteggio di similarità < 1. Dunque aggiungila"<<endl;
										cout<<"al vettore delle coppie modificanti per lo stato "<<i<<endl;
										}
									#endif

									last_modified_by_in[i][j].push_back(std::make_tuple(graph_a->SourceNode(kaddr[k]), graph_b->SourceNode(laddr[solution[k]]), sim_to_add));
								}
							}

							in_similarity+=sim_to_add;

							#ifdef DEBUG0
								if(iter==0){
									cout<<"Confronto nodo "<<i<<" del grafo A con il nodo "<<j<<" del grafo B"<<endl;
									cout<<"Punteggio sim entrante prima di dividere per il max degli archi entranti: "<<in_similarity<<endl;
								}
							#endif
						}
					}
					delete solution;
					free(costs);
				}

				if(max(klen,llen)!=0)
					in_similarity/=(max(klen,llen));
				else
					in_similarity=1;


				/* Fine similarità entrante */

				#ifdef MOD_DEBUG
//					if(iter==1){
					cout<<endl<<"Coppia ("<<i<<","<<j<<"):"<<endl;
					cout<<"modified_by_out["<<i<<"]["<<j<<"] = {";
					for(int w = 0; w < last_modified_by_out[i][j].size(); w++)
						{
							cout <<"(" << get<0>(last_modified_by_out[i][j][w]) << ", " << get<1>(last_modified_by_out[i][j][w]) << "), ";
						}
					cout<<"}"<<endl;
					cout<<"modified_by_in["<<i<<"]["<<j<<"] = {";
					for(int z = 0; z < last_modified_by_in[i][j].size(); z++)
						{
							cout <<"(" << get<0>(last_modified_by_in[i][j][z]) << ", " << get<1>(last_modified_by_in[i][j][z]) << "), ";
						}
					cout<<"}"<<endl;
//					}
				#endif
		#ifdef UPDATE_DEBUG
       		cout << "Aggiorno sim(" << i << "," << j << "): " << node_similarity[i][j] << " -> ";
		#endif

				node_similarity[i][j]=(in_similarity+out_similarity)/2;
		#ifdef UPDATE_DEBUG
        	cout << node_similarity[i][j] << endl;
		#endif
        
				// se uno qualunque tra gli elementi della matrice di similarità corrente
				// sottratto in valore assoluto all'elemento corrispondente della matrice
				// dell'iterazione passata, si mantiene maggiore o uguale a epsilon,
				// la condizione di terminazione non è soddisfata, continua a iterare
				//if(abs(tmp_similarity[i][j]-node_similarity[i][j])>=eps)
					//precision_achieved=0;	// continua a iterare
			}

		vector<vector<double> > current_similarity(graph_a_n, vector<double>(graph_b_n));

		for(int i=0; i<graph_a_n; i++)
			for(int j=0; j<graph_b_n; j++)
				current_similarity[i][j] = node_similarity[i][j];

		//history_similarity.push_back(current_similarity);


		// rimpiazza il vettore dei vettori degli stati modificanti con quello corrente
		//last_modified_by_in = temp_last_modified_by_in;
		//last_modified_by_out = temp_last_modified_by_out;

		
		#ifdef MATRICES_DEBUG
		printf("Matrice similarità corrente:\n\n");
		for(int i=0; i<graph_a_n; i++)
		{
			// Stampa matrice di similarità iterazione corrente
			std::cout<<std::endl<<"****************************************************"<<std::endl<<"Iterazione: "<<iter<<std::endl<<std::endl;
		
			printf(" [ ");
			for(int j=0; j<graph_b_n; j++)
				printf("%lf ", node_similarity[i][j]);
			printf("]\n");
		}
		printf("\n\nMatrice similarità precedente iterazione:\n\n");
		for(int i=0; i<graph_a_n; i++)
		{
			printf(" [ ");
			for(int j=0; j<graph_b_n; j++)
				printf("%lf ", tmp_similarity[i][j]);
			printf("]\n");
		}
		#endif

		#ifdef HISTORY_DEBUG
		if(iter==3){
			printf("\n\nMatrici similarità storiche:\n\n");
			for(int z = 0; z < history_similarity.size(); z++){
				cout<<"Iterazione: "<<z<<endl<<endl;
				for(int i=0; i<graph_a_n; i++)
				{
					//printf("Iterazione: %d", z, "\n\n");
					printf(" [ ");
					for(int j=0; j<graph_b_n; j++)
						printf("%lf ", history_similarity[z][i][j]);
					printf("]\n");
				}
			}
		}
		#endif

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

		last_modified_by_in = vector<vector<vector<tuple<int, int, double> > > > (graph_a_n, vector<vector<tuple<int,int,double> > >(graph_b_n, vector<tuple<int, int, double> >()));
		last_modified_by_out = vector<vector<vector<tuple<int, int, double> > > > (graph_a_n, vector<vector<tuple<int,int,double> > >(graph_b_n, vector<tuple<int, int, double> >()));
		node_similarity=new double*[graph_a_n];
		tmp_similarity=new double*[graph_a_n];
		//second_to_last_similarity=new double*[graph_a_n];
		for(int i=0; i<graph_a_n; i++)
		{
			node_similarity[i]=new double[graph_b_n];

			tmp_similarity[i]=new double[graph_b_n];
			//second_to_last_similarity[i]=new double[graph_b_n];

			for(int j=0; j<graph_b_n; j++){
				node_similarity[i][j]=graph_a->Label(i)==graph_b->Label(j);
				//second_to_last_similarity[i][j]=graph_a->Label(i)==graph_b->Label(j);
			}
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
			it=Iteration(eps, iter);
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


inline char* get_incidence_matrix(vector<map<string,int>> ttable, int num_states, vector<string> alphabet){

	char* incidence_matrix=new char[num_states*num_states];

	for(int i=0;i<(num_states*num_states);++i)
		incidence_matrix[i]=0;

	int arrive_state=0;
	for(int row=0;row<num_states;++row)
		for(string sym : alphabet){
			arrive_state=ttable[row][sym];
			incidence_matrix[row*num_states+arrive_state]=1;
		}

	return incidence_matrix;

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
