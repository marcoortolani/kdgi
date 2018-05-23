/*
 * bluefringe.cc
 */

 
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <map>
#include <cmath>
#include <limits>


#include <cstdlib>  // For exit() function

#ifdef HAVE_OPENMP
#include "omp.h"	//OpenMP
#endif

#include "bluefringe.h"
#include "utilities.h"


using namespace std;

/* Convezioni:
 *	 colonna[dim_alfabeto]   -> tipo;
 *	 tipo:   0-> neutro, 1-> accettante, 2-> rigettante, 3-> eliminato
 */
#define STATE_TYPE_COLUMN = "dim_alfabeto"


//TODO: cambiare in una define (così puoi attivarlo e disattivarlo in compilazione)
//bool db = false;													// Attiva DEBUG

BlueFringe::BlueFringe(const char * path){
	path_samples_ = new char[strlen(path)+1];

	num_actual_merge_ =0;
	num_heuristic_merge_valued_=0;

	fringe_size_[0].reserve(1000); 		// Size of blue fringe
	fringe_size_[1].reserve(1000);			// Size of red fringe

	strcpy(path_samples_, path);

	alphabet_	= vector<string>();
	while_count_ = -1;
}


BlueFringe::~BlueFringe()
{

	alphabet_.clear();

	delete[] path_samples_;

}


void BlueFringe::read_samples(vector<string>* &positive, int* dim_positive, vector<string>* &negative,  int *dim_negative,int* &wp, int* &wn)
{
	#ifdef VERBOSE
	cout << "Reading strings from txt file: "<<endl;
	#endif
	int cp = 0;														// Numero di stringhe positive del linguaggio
	int cn = 0;														//   -    -     -     negative  -      -
	char ch;
	int dim_alphabet;

	string null_symbol;
	#ifdef VERBOSE
	cout << path_samples_ << endl;
	#endif

	fstream fin(path_samples_, fstream::in);
	if(!fin){
		cerr<<"An error occurred in opening file :("<<endl;
		exit(EXIT_FAILURE);
	}

	while (fin >> noskipws >> ch) {									// Faccio in conteggio previo del numero di stringhe positive e negative presenti nel txt
		if(ch == '\n')
			continue;
		else if(ch == '+')
			cp++;
		else if(ch == '-')
			cn++;
	}
	(*dim_positive) = cp;
	(*dim_negative) = cn;

	positive = new vector<string>[cp];
	negative = new vector<string>[cn];

	wp	= new int[cp];
	wn	= new int[cn];
	for(int i=0; i<cp; ++i)
		wp[i] = 0;
	for(int i=0; i<cn; ++i)
		wn[i] = 0;

	#ifdef VERBOSE
	cout << intTostring(cp) + " positivi"<<endl;
	cout << intTostring(cn) + " negativi"<<endl;
	#endif

	int flagcp = 0;
	int flagcn = 0;
	bool casopositive = false;
	bool casonegative = false;
	bool primap = true;
	bool priman = true;

	ifstream infile(path_samples_);

	bool first = true;
	bool second = false;
	bool weights = false;
	bool weight = false;
	string line;

	while (getline(infile, line))
	{
	    istringstream iss(line);
	    int a;
	    string n;
		char w;


	    // Read first line for dim alphabet
	    if(first)
	    {
	    	if (!(iss >> a)) { break; } // error
	    	//cout << "dimensione alfabeto " << a << endl;
			dim_alphabet = a;
	    	first = false;
	    	second = true;
			if((iss >> w) && w == 'w')
				weights = true;

	    	continue;
	    }


	    // Read second line for alphabet symbol
	    if(second)
	    {
			//cout<<"sono dentro if(second)"<<endl;
	    	int counter=-1;
	    	while (iss >> n){
	    		if(counter==-1){
	    			null_symbol = n;
	    			++counter;
	    			continue;
	    		}else if(counter>=dim_alphabet)
	    			break;
				//cout<<"aggiungo all'alfabeto la lettera: "<<n<<endl;
	    		alphabet_.push_back(n);
				++counter;
	    	}

	    	// Alphabet
	    	if(counter!= alphabet_.size()){
	    		cerr<<"Error in reading example: number of red alphabet symbols mismatches with the declared one!"<<endl;
	    		cerr<<"Expected symbols: "<<alphabet_.size()<<endl;
	    		cerr<<"Read symbols: "<<counter<<endl;


	    		exit(EXIT_FAILURE);
	    	}

	    	// alphabet ok ;)
	    	second= false;
	    }

	    // Read remaining lines
		while (iss >> n)
		{
			//cout<<"Ho letto: "<<n<<endl;
			if( !n.compare("+") ){
				if(weights)	weight=true;
				casopositive = true;
				casonegative = false;
				if(primap){												// Se è il primo caso evito l'incremento
					primap =false;
					continue;
				}
				flagcp++;
				continue;

			}else if( !n.compare("-") ){
				if(weights)	weight=true;
				casonegative = true;
				casopositive = false;
				if(priman){												// Se è il primo caso evito l'incremento
					priman =false;
					continue;
				}
				flagcn++;
				continue;
			}


			if(weight){
				//cout<<"Sono dentro if(weight) a riga 206: n="<<n<<endl;
				weight = false;
				if(casopositive)
					wp[flagcp] = stringToint(n);
				else if(casonegative)
					wn[flagcn] = stringToint(n);

				// DEBUG
				//cout << "!T Peso: "<< stringToint(n) << endl;
			} 
			else{
				//cout<<"Sono dentro else a riga 218: n="<<n<<endl;

				// se la stringa è vuota, non è necessario aggiungere nulla
				if(n.compare(null_symbol) == 0){
					continue;
				}

				if(casopositive)
							positive[flagcp].push_back(n);
				else if(casonegative)
							negative[flagcn].push_back(n);
			}
		}
	}
	/*
	cout<<"positive.size()="<<positive->size()<<" dim_positive=" <<*dim_positive<<endl;
	for(int i=0; i<*dim_positive;++i)
		cout<<"positive[i]="<<positive[i]<<endl;
	for(int i=0; i<*dim_negative;++i)
		cout<<"positive[i]="<<negative[i]<<endl;
	*/
	if(!weights){
		//inizializzo wp e wn a 1
		for(int i=0;i<cp;++i)
			wp[i]=1;
		for(int j=0;j<cn;++j)
			wn[j]=1;
	}
}


// build apta from sample set
RedBlueDfa* BlueFringe::build_apta(const vector<string>* positive, const int dim_positive, const vector<string>* negative, const int dim_negative)
{
	// Flag per rilevare la presenza della stringa vuota (positiva o negativa)
	bool empty_string_positive = false;
	/*
	cout << "**** STRINGHE ****"<<endl;
	for(int i=0; i< dim_positive; ++i)
		cout << "i: "<<i<<"  "<<positive[i]<<endl;
	for(int i=0; i< dim_negative; ++i)
		cout << "i: "<<i<<"  "<<negative[i]<<endl;
	*/

	#ifdef VERBOSE
	cout << "Costruzione APTA..."<<endl;
	#endif

	// PREFISSI
	// Calcolo i prefissi e li salvo insieme ad un indice indicatore dello stato
	map<vector<string>,int> prefissi;
	typedef	map<vector<string>,int>::const_iterator It;

	typedef	vector<string>::const_iterator It_posneg;

	// add empty string
	vector<string> emptyVec(0);
	prefissi[emptyVec] = 0;
	//cout<<"dim positive="<<dim_positive<<" dim negative="<<dim_negative<<endl;
	for(int i=0; i<dim_positive; ++i){
		if(positive[i].size() == 0){
			//cout<<"empty_string_positive"<<endl;
			empty_string_positive = true;
			continue;
		}

		It_posneg w = positive[i].begin();
		for(It_posneg j=positive[i].begin(); j!=positive[i].end(); ++j){

			vector<string>   sub(w,j+1);

			if(prefissi[sub] == 0)
				prefissi[sub] = ND;			// Qui e dopo sono 0 perché non li setto adesso!
		}
	}

	for(int i=0; i<dim_negative; ++i){
		if(negative[i].size() == 0){
			//cout<<"empty_string_negative"<<endl;
			continue;
		}

		It_posneg w = negative[i].begin();
		for(It_posneg j=negative[i].begin(); j!=negative[i].end(); ++j){
			vector<string>   sub(w,j+1);
			if(prefissi[sub] == 0)
				prefissi[sub] = 0;
		}
	}

	// Assegno gli indici di stato corretti per i relativi prefissi
	int indice_stato=0;
	for(It p1=prefissi.begin(); p1!=prefissi.end(); ++p1){
		prefissi[(*p1).first] = indice_stato;
		++indice_stato;
	}

	/* Codice verifica prefissi*/
//	if(false)
//		for(It p1=prefissi.begin(); p1!=prefissi.end(); ++p1){
//			for(It_posneg i=p1->first.begin(); i<p1->first.end(); ++i)
//				cout << (*i);
//			cout<< "; " << p1->second << endl;
//		}

	// *** PTA ***
	RedBlueDfa* aptaDFA = new RedBlueDfa(prefissi.size(), alphabet_,0);
	bool primo_stato=true;
	for(It p=prefissi.begin(); p!=prefissi.end(); ++p)
	{
		vector<string> stato = (*p).first;
		//cout << "Stato considerato:"<<stato<<endl;


		// Solo per il Primo Stato -gestisco a parte l'essere red e intanto non accettante-
		// Controllo per ogni elemento dell'alfabeto, per quali è definita una transizione dallo stato iniziale
		if(primo_stato)
		{
			for(string i : alphabet_){							// Setto il valore delle Transizioni
				vector<string> symbol;
				symbol.push_back(i);
				if(prefissi.find(symbol) != prefissi.end())				// Controllo che sia definito uno stato per quando entra dopo lambda un elemento dell'alfabeto
					aptaDFA->set_ttable_entry((*p).second,i,prefissi[ symbol ]);
			}

			if(empty_string_positive)
				aptaDFA->set_accepting_states_entry((*p).second, 1);							// Primo stato: accettante
			else
				aptaDFA->set_accepting_states_entry((*p).second, 0);
			aptaDFA->add_red_state((*p).second);						// Aggiungo lo stato rosso anche alla gestione tramite VECTOR

			primo_stato=false;
			continue;
		}



		// Stati Accettanti
		for(int i=0; i<dim_positive; ++i){								// Pongo gli stati opportuni ad accettanti: quelli la cui stringa è anche tra le stringhe positive
			if(positive[i].size() == 0){
				aptaDFA->set_accepting_states_entry(0, 1);								// TODO: Check and debug
				continue;
			}

			if(stato == positive[i]){
				aptaDFA->set_accepting_states_entry((*p).second, 1);						// Con dim_alfabato arrivo alla prima colonna oltre le lettere dell'alfabeto
				break;
			}
			else
				aptaDFA->set_accepting_states_entry((*p).second, 0);
		}


		// Transizioni
		for(string sym : alphabet_){						// Verifico per ogni lettera dell'alfabeto che esista la transizione; se esiste memorizzo
			vector<string> temp_vect(stato);
			temp_vect.push_back(sym);
			if(prefissi.find(temp_vect) != prefissi.end())		// nella riga dello stato considerato ((*p).second) la relativa transizione
				aptaDFA->set_ttable_entry((*p).second,sym, prefissi[temp_vect]);
		}

	}

	//Coloro gli stati blu (il primo livello dopo lambda in ESDM):
	for(string sym : alphabet_)
	{
		int statoblu = aptaDFA->get_ttable(0,sym);
		if(aptaDFA->get_ttable(0,sym) != ND)
			aptaDFA->add_blue_state(statoblu);							// Lo aggiungo anche al Vector di stati blu
	}

	// Stampo APTA
	/*aptaDFA->print_dfa_with_color("*** APTA DENTRO ***");
	aptaDFA->print_dfa_dot("apta",PATH_DOT_APTA);*/
	#ifdef VERBOSE
	cout << "PTA costruito"<<endl;
	#endif

	return aptaDFA;
}


void BlueFringe::merge(RedBlueDfa* dfa1, int redstate, int blustate)
{
	//dfa1->print_dfa_ttable("dfa1 - merge");
	int predecessore=0;
	string lettera;
	int num_predecessori=0;

	// Cerco il predecessore dello stato blue
	for(int i=0; i<dfa1->get_num_states(); ++i)
	{
		for(string sym : alphabet_)
			if(dfa1->get_ttable()[i][sym] == blustate){
				predecessore = i;
				lettera = sym;
				num_predecessori++;
			}
	}
	//cout<<"predecessori stato blue"<<endl;

	if(num_predecessori != 1)
		cerr << "Num predec:"<< num_predecessori<<" PROBLEMA: "<< num_predecessori << "per lo stato "<<blustate<<", controlla algoritmo!"<<endl;

	// Setto la transizione del precedessore (di bluestate) a redstate,
	// adesso q2 è IRRAGIUNGIBILE nell'albero originale
	dfa1->set_ttable_entry(predecessore,lettera,redstate);
	fold(dfa1, redstate, blustate);
}


void BlueFringe::fold(RedBlueDfa* originale, int redstate, int blustate)
{
	vector<map<string,int>> current_ttable = originale->get_ttable();
	vector<int> accepting = originale->get_accepting_states();
	// Se q2 è accettante, setto ad altrettanto q1
	if(originale->is_accepting(blustate)){
		originale->set_accepting_states_entry(redstate, DFA_STATE_ACCEPTING);
	}

	// Per ogni lettera, effettuo il controllo se nell'albero originale è definita
	// una transizione presente anche nel subtree:
	// se presente passo agli stati successivi, diversamente effettuo il merge inserendo la transizione.
	for(string sym : originale->get_alphabet())
	{
		int statefrom_bluestate = current_ttable[blustate][sym];
		int statefrom_redstate = current_ttable[redstate][sym];

		// Ormai posso eliminare lo stato blue, lo faccio settando a ND la sua transizione per la lettera corrente
		originale->set_ttable_entry(blustate,sym, ND);

		// TODO: Potrei già eliminare qui lo stato dai blue? penso di si, ma fai debug prima di attivarlo
		//current_ttable[blustate][colonna_tipo+1] = DFA_STATE_WHITE;
		//originale->remove_blue_state(blustate);
		if(statefrom_bluestate != ND){
			if(statefrom_redstate != ND)
				fold(originale, statefrom_redstate, statefrom_bluestate);
			else{
				// Aggiungo la transizione esistente nel subtree ma non nello stato in cui faccio il merge
				originale->set_ttable_entry(redstate,sym,statefrom_bluestate);
				//current_ttable[blustate][i] = ND;
			}
		}
	}
	// Non c'è bisogno che elimini la riga dello stato, sarà irragiungibile da tutti, puoi tenerne conto alla fine.
}



void BlueFringe::promote(RedBlueDfa* dfa1, int q)
{
	if(!dfa1->is_inside_blue_states(q))
		cerr << "ATTENZIONE! Lo stato non è BLU!"<< endl;

	// Promuovo a RED lo stato q
	dfa1->add_red_state(q);

	dfa1->remove_blue_state(q);

	// Promuovo a BLU tutti gli stati raggiungibili direttamente da q
	for(string sym : dfa1->get_alphabet())
	{
		int transizione = dfa1->get_ttable(q,sym);
		if(transizione != ND)
			dfa1->add_blue_state(transizione);
	}
}



// Promuovo a BLU tutti gli stati raggiungibili in modo diretto da un RED
void BlueFringe::nuoviBlu(RedBlueDfa* dfa1)
{
	int numred = dfa1->get_num_red_states();

	// Promuovo a BLUE tutti gli stati raggiungibili da un RED in modo diretto
	for(string sym : dfa1->get_alphabet())
	{
		for(int j=0; j<numred; ++j)
		{
			int red = dfa1->get_red_states()->at(j);
			int transizione = dfa1->get_ttable(red,sym);
			if(transizione != ND){
				//Pongo lo stato a blu, a meno che non sia rosso
				if(!dfa1->is_inside_red_states(transizione)){
					dfa1->add_blue_state(transizione);

				}
			}
		}
	}
}

void BlueFringe::eliminaStati(RedBlueDfa* dfa1)
{
	vector<int> accepting_states = dfa1->get_accepting_states();

	// Setto gli stati Eliminati
	for(int k=1; k<dfa1->get_num_states(); ++k)				// Verifico se lo stato K è visitato da qualcuno
	{
		bool visitato = false;
		for(int i=0; i<dfa1->get_num_states(); ++i)
		{
			for(string sym : dfa1->get_alphabet())
				if(dfa1->get_ttable(i,sym) == k && accepting_states[i] != DFA_STATE_UNREACHABLE)
					visitato = true;
		}

		if(!visitato){
			dfa1->set_accepting_states_entry(k,DFA_STATE_UNREACHABLE);		//Setto a Eliminato il tipo dello stato non visitato da alcun altro stato

			// Li elimino anche dal Vector dei Red o dei Blue, se presenti
			dfa1->remove_red_state(k);
			dfa1->remove_blue_state(k);
		}
	}

}


int BlueFringe::get_actual_merge(){
	return num_actual_merge_;
}

int BlueFringe::get_heuristic_merge(){
	return num_heuristic_merge_valued_;
}

void BlueFringe::set_fringe_size(int r, int b){
	fringe_size_[0].push_back(b);		// 0 for Blue, 1 for Red
	fringe_size_[1].push_back(r);

}

void BlueFringe::print_fringe_size(){

	vector<int>::const_iterator i;
	vector<int>::const_iterator l;

	i=fringe_size_[1].begin();			// 0 for Blue, 1 for Red
	l=fringe_size_[0].begin();

	while(i!=fringe_size_[1].end()){

		cout<<"R:"<<(*i)<<",";
		cout<<"B:"<<(*l) << endl;

		i++;
		l++;
	}

	cout <<"-------"<<endl;
}

int BlueFringe::get_while_count(){
	return while_count_;
}
