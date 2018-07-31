/*
 * rpni.cc
 */
 

#include "rpni.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <map>
#include <cmath>
#include <limits>
#include <unistd.h>
#include <sys/time.h>
#include <chrono>
#include <ctime>


#include <cstdlib>  // For exit() function

#ifdef HAVE_OPENMP
#include "omp.h"	//OpenMP
#endif

#include "utilities.h"



#define ND numeric_limits<int>::max()
#define MINF numeric_limits<int>::min()


using namespace std;

/* Convezioni:
 *	 colonna[dim_alfabeto]   -> tipo;
 *	 tipo:   0-> neutro, 1-> accettante, 2-> rigettante, 3-> eliminato
 */
#define STATE_TYPE_COLUMN = "dim_alfabeto"


//TODO: cambiare in una define (così puoi attivarlo e disattivarlo in compilazione)
//bool db = false;													// Attiva DEBUG


Rpni::Rpni(const char * path):BlueFringe(path){};

//TODO: verificare che venga invocato il distruttore della classe "bluefringe"
Rpni::~Rpni(){};


int Rpni::merge_heuristic_score(RedBlueDfa* dfa1, vector<string>* positive, int dim_positive, vector<string>* negative, int dim_negative, int* wp = NULL, int* wn=NULL)
{
	for(int i=0; i<dim_negative; ++i)
		if(dfa1->membership_query(negative[i]))
			return 0;

	return 1;
}


ConcreteDfa* Rpni::run(string base_path, double exec_time)
{
	// Samples from txtfile
//	int n_symbols=0;				//
	int dim_positive=0;			// number of positive examples
	int dim_negative=0; 			// number of negative examples
	
//	int max_count;
	int *curr_count=NULL;
	int j_merge=ND;
	
	int	n_red=0, n_blue=0, actual_blue_state=0;

	// example strings
	vector<string>* positive=NULL;
	vector<string>* negative=NULL;
	char* symbols = NULL;
	int* wp, *wn;
	
	//bool *promotion=NULL;
	bool promoted =false;
	
	RedBlueDfa *dfa1 =NULL, /* *dfa_best = NULL,*/ **merged = NULL; // dfa...

	// One dfa_best for every Blue state
	vector<RedBlueDfa*> dfa_best;
	vector<int> dfa_score;


	//Get positive and negative samples
	read_samples(positive, &dim_positive, negative, &dim_negative, wp, wn);

	#ifdef VERBOSE
	 cout << "**** STRINGHE ****"<<endl;
	 for(int i=0; i< dim_positive; ++i)
	 	cout << "i: "<<i<<"  "<<positive[i]<<endl;
	 for(int i=0; i< dim_negative; ++i)
	 		cout << "i: "<<i<<"  "<<negative[i]<<endl;
	#endif

	/////////////////////////////////
	// START TIME

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	

	// Costruisco PTA
	//RedBlueDfa* dfa1 = build_pta(positive,dim_positive);
	
	// Build APTA
	dfa1 = build_apta(positive, dim_positive, negative, dim_negative);
	// Print it!
	if(dfa1->get_num_states() < 1000)
	{
		dfa1->print_dfa_dot("APTA", (base_path+"APTA.dot").c_str() );
	}else{
		clog<<"APTA too big! I can't print it"<<endl;
	}

	n_blue = dfa1->get_num_blue_states();
	n_red = dfa1->get_num_red_states();

	set_fringe_size(n_red,n_blue);

	#ifdef VERBOSE
	cout <<" START RPNI inference process..."<<endl;
	#endif

	while_count_=-1;
	// RPNI
	while(n_blue>0)
	{		
		while_count_++;
		

		// BLUE ciclo
		for(int i=0; i<n_blue; ++i)
		{	
			//T cout << "N blue states: "<< dfa1->get_num_blue_states() << ", i: "<<i<< endl;
			actual_blue_state = dfa1->get_blue_states()->at(i);
			///// Reset variable for the new run ////
			// array for the heuristic values of the red group
			if(curr_count != NULL)
				delete[] curr_count;
			curr_count= new int [n_red];
			
			// dfa coming from possible merges
			if(merged != NULL)
				delete[] merged;
			merged = new RedBlueDfa*[n_red];
			
			// initialize values
			for(int j=0; j<n_red; ++j){
				curr_count[j] = false;
				merged[j] = NULL;
			}			
			
			// RED ciclo
			#pragma omp parallel default(shared)
			{
			#pragma omp for
			for(int j=0; j<n_red; ++j){
				merged[j] = new RedBlueDfa(*dfa1);
				
				merge(merged[j], dfa1->get_red_states()->at(j), actual_blue_state );
				// TODO: Questa riga si può probabilmente eliminare, da fare debug estensivo
				merged[j]->remove_blue_state(actual_blue_state);

				curr_count[j] = merge_heuristic_score(merged[j], positive, dim_positive, negative, dim_negative);
			}
			}
			// end for RED

			// For Statistical purpose
			num_heuristic_merge_valued_ +=  n_red;
			
			// check if there some merge, else start process for promote
			promoted = true;
			j_merge=ND;
			for(int j=0; j<n_red; ++j){
				if(curr_count[j]>0 && promoted)
				{
					j_merge = j;

					promoted=false;

					//T cout << "Merge detected!"<<endl;
				} else {
					if(merged[j] != NULL)
						delete merged[j];
				}
			}

			break;
		}// end for BLUE


		// PROMOTION
		if(promoted){
			//T cout << "PROMOZIONE"<<endl;
			// "Promote" add also the new blue states
			promote(dfa1, actual_blue_state);

		// MERGE
		} else {
			dfa1 = merged[j_merge];

			promoted=false;
			nuoviBlu(dfa1);

			//T cout << "MERGE"<<endl;
		}


		eliminaStati(dfa1);

		delete[] merged;
		merged = NULL;
		
		//T cout << "New blue states..."<<endl;
		// update values for the dfa
		n_blue = dfa1->get_num_blue_states();
		n_red = dfa1->get_num_red_states();

		set_fringe_size(n_red,n_blue);
	}
	

	if(curr_count != NULL) delete[] curr_count;
	

	// Setto gli stati Accettanti
//	int colonna_tipo = dfa1->get_dim_alphabet();
//	for(int i=0; i<dim_positive; ++i){
//		int accettante = dfa1->get_arrive_state(positive[i]);

//			if(accettante != ND)
//				dfa1->get_ttable()[accettante][colonna_tipo] = DFA_STATE_ACCEPTING;
//	}

	// Setto gli stati Rigettanti
	for(int i=0; i<dim_negative; ++i){
		int rigettante = dfa1->get_arrive_state(negative[i]);
		if(rigettante != ND){
			//cout << "Stato di arrivoN: "<<rigettante<<endl;
			dfa1->set_accepting_states_entry(rigettante, 0);
		}
	}

	// Setto gli stati Eliminati
	eliminaStati(dfa1);
	//dfa1->print_dfa_with_color("AUTOMA FINALE");

	///////////////////////////////////////////////////////////////
	// Delete the unreachable states and insert, if needed, the sink state
	RedBlueDfa* finalDFA = dfa1->to_canonical_RedBlueDfa_from_red_states();
	//////////////////////////////////////////////////////////////
	// Minimize returns a new dfa, then delete the older
	ConcreteDfa* finalDFAmin = finalDFA->minimize_TF();
	
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	if(exec_time!=-1){
		exec_time=std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
	}

	if(finalDFA) delete finalDFA;

	if(positive) delete[] positive;
	if(negative) delete[] negative;
	if(symbols) delete[] symbols;


	return finalDFAmin;

}