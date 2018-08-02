/*
 * dfa.tcc
 */

template <class I>
Dfa<I>::Dfa(){
	//TO-DO: implementare questo controllo per ogni costruttore di Dfa (spostare quelli non necessari in ConcreteDfa)
	//Nota bene: questo controllo permette di usare solo i container sequenziali, in quanto quelli associativi richiedono
	//un ordinamento forte (non deve essere possibile l'uguaglianza su elementi diversi(?)) che non abbiamo definito per DfaState
	
	DfaState test(true, vector<symbol_>(), 0);
	static_cast<typename basetype::value_type>(test);
	
	num_states_   = 0;
	start_state_  = 0;
	alphabet_	= vector<symbol_>();
}


template <class I>
Dfa<I>::Dfa(const int n_state, const vector<symbol_> alf, const int s_state){
	num_states_   = n_state;
	start_state_  = s_state;
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
}

template <class I>
Dfa<I>::Dfa(const int n_state, const vector<symbol_> alf)
:Dfa(n_state, alf, 0){}

template <class I>
Dfa<I>::~Dfa()
{
	alphabet_.clear();
}

template <class I>
void Dfa<I>::set_num_state(int n){
	num_states_ = n;
}

template <class I>
void Dfa<I>::set_alphabet(const vector<symbol_> alf)
{
	alphabet_.clear();
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
}

template <class I>
int Dfa<I>::get_dim_alphabet() const{
	return alphabet_.size();
}

template <class I>
const vector<symbol_> Dfa<I>::get_alphabet() const{
	return alphabet_;
}

template <class I>
int Dfa<I>::get_num_states() const{
	return num_states_;
}

template <class I>
int Dfa<I>::get_start_state() const{
	return start_state_;
}

/*
template <class I>
vector<symbol_> Dfa<I>::table_filling1(Dfa* subject_dfa) {
	
	int total_states = num_states_ + subject_dfa->get_num_states();

	map<int, DfaState> indexes;

	int i = 0;
	for(auto state : *this){
		indexes[i] = state;
		++i;
	}
	for(auto state : subject_dfa){
		indexes[i] = state;
		++i;
	}

	for(auto state : *this){
		cout<<*(indexes.find(state));
	}
	for(auto state : subject_dfa){
		cout<<*(indexes.find(state));
	}
	

}
*/



/* Methods related to the "dfa common interface" */

template <class I>
vector<symbol_> Dfa<I>::sort_alphabet(){
	//bubblesort
	vector<symbol_> sorted_alphabet = get_alphabet();
	bool sorted = false;

	while(!sorted){
		sorted = true;
		for(int i = 0; i < sorted_alphabet.size() - 1 && sorted; ++i){
			if(!lexicographical_compare(sorted_alphabet.begin() + i, sorted_alphabet.begin() + i+1, sorted_alphabet.begin() + i+1, sorted_alphabet.begin() + i+2)){
				sorted = false;
				symbol_ temp = sorted_alphabet[i];
				sorted_alphabet[i] = sorted_alphabet[i+1];
				sorted_alphabet[i+1] = temp;
			}
		}
	}
	return sorted_alphabet;
}

template <class I>
bool Dfa<I>::is_identical(Dfa* other_dfa, vector<symbol_>& phrase){
	vector<symbol_> sorted_alphabet = sort_alphabet();
	
	auto state_it = this->begin();
	auto other_state_it = other_dfa->begin();
	  
	while(state_it != this->end() && other_state_it != other_dfa->end()){
		DfaState state = *state_it;
		DfaState other_state = *other_state_it;
			  
		if(state.get_depth_phrase() != other_state.get_depth_phrase() || state.is_accepting() != other_state.is_accepting()){
			
			if(state.get_depth_phrase() == other_state.get_depth_phrase()){
				phrase = state.get_depth_phrase();
			}
			else{
				phrase = lexicographical_compare(state.get_depth_phrase().begin(), state.get_depth_phrase().end(), other_state.get_depth_phrase().begin(), other_state.get_depth_phrase().end()) ? state.get_depth_phrase() : other_state.get_depth_phrase();
			}
			
			return false;
		}
			  
		for(symbol_ sym : sorted_alphabet){
			DfaState* next_state = state.next(sym);
			DfaState* other_next_state = other_state.next(sym);
			
			if(next_state->get_depth_phrase() != other_next_state->get_depth_phrase() || next_state->is_accepting() != other_next_state->is_accepting()){
				
				phrase = state.get_depth_phrase();
				phrase.push_back(sym);
				
				return false;
			}
		}
		++state_it;
		++other_state_it;
	}
	if(state_it != this->end() || other_state_it != other_dfa->end()){
		if(state_it != this->end()){
			phrase = state_it->get_depth_phrase();
		}
		else{
			phrase = other_state_it->get_depth_phrase();
		}
		return false;
	}
	  
	return true;
};

template <class I>
vector<symbol_> Dfa<I>::get_next_phrase(vector<symbol_> phrase, bool strict){
	vector<symbol_> sorted_alphabet = sort_alphabet();
	
	if(phrase.empty()){
		if(strict == false)
			return phrase;
		cerr << "Error in Dfa :: next_phrase: argument has length 0" << endl;
		throw 0;
	}
	
	if(phrase.back() == sorted_alphabet.back()){
		while(phrase.back() == sorted_alphabet.back()){
			phrase.pop_back();
		}
		
		if(phrase.empty()){
			if(strict == false)
				return phrase;
			cerr << "Error in Dfa :: next_phrase: phrase is now empty" << endl;
			throw 0;
		}
	}
	
	symbol_ last_sym = phrase.back();
			
	bool found_symbol = false;
	for(auto next = sorted_alphabet.begin(); next != sorted_alphabet.end() && !found_symbol; ++next){
		if(*next == last_sym){
			found_symbol = true;
			++next;
			phrase.pop_back();
			phrase.push_back(*next);
		}
	}
	
	return phrase;
}

template <class I>
template <class SymIter>
DfaState* Dfa<I>::operator[](SymIter phrase){
	return (*begin())[phrase];
}

template <class I>
vector<vector<double>> Dfa<I>::neighbour_matching(Dfa* subject_dfa, bool isomorphism, bool color, double eps){

	int max_iter = 10000;


	int num_states_subject_ = subject_dfa -> get_num_states();

	// similarità corrente
	double **node_similarity;

	// ultima iterazione
    double **tmp_similarity;


	// INIT
	node_similarity=new double*[num_states_];
	tmp_similarity=new double*[num_states_];

	for(DfaState state_reference : *this)
	{
		node_similarity[state_reference.get_index()]=new double[num_states_subject_];

		tmp_similarity[state_reference.get_index()]=new double[num_states_subject_];

		for(DfaState state_subject : *subject_dfa){
			if(color)
				node_similarity[state_reference.get_index()][state_subject.get_index()]=state_reference.is_accepting()==state_subject.is_accepting();
			else
				node_similarity[state_reference.get_index()][state_subject.get_index()]=1;
		}
	}

	vector<vector<vector<tuple<int, int, double> > > > last_modified_by_in;
	vector<vector<vector<tuple<int, int, double> > > > last_modified_by_out;

	last_modified_by_in = vector<vector<vector<tuple<int, int, double> > > > (num_states_, vector<vector<tuple<int,int,double> > >(subject_dfa->num_states_, vector<tuple<int, int, double> >()));
	last_modified_by_out = vector<vector<vector<tuple<int, int, double> > > > (num_states_, vector<vector<tuple<int,int,double> > >(subject_dfa->num_states_, vector<tuple<int, int, double> >()));

	long *solution;
    long *costs;


	int iter=0;
	int precision_achieved=0;
	while(!precision_achieved && iter<max_iter)
	{
		// flag per fermare l'iterazione, 1 significa che la condizione di terminazione è soddisfatta
		precision_achieved=1;

		double sim_to_add = 0;

		for(int i=0; i<num_states_; i++)
			for(int j=0; j<num_states_subject_; j++){
				// l'ultima iterazione è uguale a quella corrente prima di effettuare un qualsiasi conto
				tmp_similarity[i][j]=node_similarity[i][j];
			}

		for(auto state_it = this->begin(); state_it != this->end(); ++state_it){
			DfaState state_reference = *state_it;
			int i = state_reference.get_index();
			for(auto other_state_it = subject_dfa->begin(); other_state_it != subject_dfa->end(); ++other_state_it){
				DfaState state_subject = *other_state_it;
				int j = state_subject.get_index();

				if(color)
					if(state_reference.is_accepting()!=state_subject.is_accepting())
						continue;

				// nodi del vicinato: kaddr == vicinato del nodo del grafo A; laddr == vicinato del nodo del grafo B
				//vector<int>::iterator kaddr,laddr;
				vector<DfaState*> vicinato_uscente_reference = state_reference.get_outcoming_states();

				
				int cardinalita_vicinato_uscente_reference = vicinato_uscente_reference.size();

				vector<DfaState*> vicinato_uscente_subject = state_subject.get_outcoming_states();

				int cardinalita_vicinato_uscente_subject = vicinato_uscente_subject.size();

				double out_similarity=0;

				if(cardinalita_vicinato_uscente_reference>0 && cardinalita_vicinato_uscente_subject>0){

					// costs è la sottomatrice costituita dal vicinato (entrante o uscente)
					// che contiene i pesi, cioe i punteggi di similarità internodo,
					// è in sostanza la matrice su cui risolvere l'assignment problem per
					// trovare il matching ottimo

					costs=(long *)malloc(cardinalita_vicinato_uscente_reference*cardinalita_vicinato_uscente_subject*sizeof(long));

					solution=new long[cardinalita_vicinato_uscente_reference];



					for(int k=0; k<cardinalita_vicinato_uscente_reference; k++)
						for(int l=0; l<cardinalita_vicinato_uscente_subject; l++)
							// l'hungarian algorithm di suo risolve il MINIMUM assignment problem
							// noi siamo interessati al MAXIMUM assignment problem, ecco che
							// prendiamo il complementare rispetto a 1 di ciascun peso.
							costs[k*cardinalita_vicinato_uscente_subject+l]=(1-tmp_similarity[vicinato_uscente_reference[k]->get_index()][vicinato_uscente_subject[l]->get_index()])/eps;

					// solution è un vettore di dimensione pari alla cardinalità del vicinato del nodo
					// del grafo A, e cioè i della corrente iterazione.
					// l'indice con cui si accede a solution indica il nodo del vicinato di i appartenente al grafo A
					// (indice scalato alla sottomatrice che comprende solo il vicinato, kaddr ed laddr consentono di recuperare il nodo vero a partire dall'indice scalato)
					// il valore contenuto nel vettore indica quale nodo del vicinato di j appartenente al grafo B
					// si deve associare all'indice per avere il matching ottimo.
					// un valore negativo indica che il nodo i non fa parte del matching
					hungarian(&costs, cardinalita_vicinato_uscente_reference, cardinalita_vicinato_uscente_subject, solution, 0);

					for(int k=0; k<cardinalita_vicinato_uscente_reference; k++){

						if(solution[k]>=0){

							std::vector<std::tuple<int, int, double> >:: iterator it ;
							bool already_modified = false;

							// Controlla se la coppia del matching ottimo da sommare al punteggio di similarità
							// è stata già tenuta in considerazione nell'ultima iterazione:
							// se è cosi allora pesca il punteggio di similarità della coppia dalla penultima iterazione

							for (it = last_modified_by_out[i][j].begin(); it != last_modified_by_out[i][j].end(); ++it)  /** ERRORE QUI **/
								if (get<0>(*it) == vicinato_uscente_reference[k]->get_index() && get<1>(*it) == vicinato_uscente_subject[solution[k]]->get_index()){

									sim_to_add = get<2>(*it);

									#ifdef DEBUG
										cout<<endl<<"Confronto nodi ("<<i<<", "<<j<<"): "<<endl;
										cout<<"La coppia del vicinato uscente ("<<vicinato_uscente_reference[k]->get_index()<<", "<<vicinato_uscente_subject[solution[k]]->get_index()<<"):"<<endl;
										cout<<"è già stata considerata all'iterazione precedente."<<endl<<"Considero il punteggio della iterazione STORICA precedente a quella in cui la coppia è stata aggiunta: "<<sim_to_add<<endl;
									#endif

									already_modified = true;
									break;
								}

							// se la coppia non è già intervenuta nella passata iterazione, considera
							// il loro punteggio all' ultima iterazione
							if(!already_modified){

								sim_to_add = tmp_similarity[vicinato_uscente_reference[k]->get_index()][vicinato_uscente_subject[solution[k]]->get_index()];

								#ifdef DEBUG2
									if(iter==18){
										cout<<endl<<"Confronto nodi ("<<i<<", "<<j<<"): "<<endl;
										cout<<"La coppia del vicinato uscente ("<<vicinato_uscente_reference[k]->get_index()<<", "<<vicinato_uscente_subject[solution[k]]->get_index()<<"):"<<endl;
										cout<<"NON è stata considerata all'iterazione precedente."<<endl<<"Considero il punteggio della PRECEDENTE iterazione: "<<sim_to_add<<endl<<tmp_similarity[vicinato_uscente_reference[k]->get_index()][vicinato_uscente_subject[solution[k]]->get_index()]<<endl;
									}
								#endif

								if(iter >=1 && tmp_similarity[vicinato_uscente_reference[k]->get_index()][vicinato_uscente_subject[solution[k]]->get_index()] < 1){
									last_modified_by_out[i][j].push_back(std::make_tuple(vicinato_uscente_reference[k]->get_index(), vicinato_uscente_subject[solution[k]]->get_index(), sim_to_add));
									precision_achieved = 0;	//continua a iterare
									
									#ifdef DEBUG
										cout<<endl<<"La coppia del vicinato uscente: ("<<vicinato_uscente_reference[k]->get_index()<<", "<<vicinato_uscente_subject[solution[k]]->get_index()<<")"<<endl;
										cout<<"ha un punteggio di similarità < 1. Dunque aggiungila"<<endl;
										cout<<"al vettore delle coppie modificanti per lo stato "<<i<<endl;
									#endif
								}

							}

							out_similarity+=sim_to_add;

						}
					}
					delete solution;
					free(costs);
				}

				if(max(cardinalita_vicinato_uscente_reference,cardinalita_vicinato_uscente_subject)!=0)
					out_similarity/=(max(cardinalita_vicinato_uscente_reference,cardinalita_vicinato_uscente_subject));
				else
					out_similarity=1;

				/*** Similarita entrante ***/

				vector<DfaState*> vicinato_entrante_reference = state_reference.get_incoming_states();
				vector<DfaState*> vicinato_entrante_subject = state_subject.get_incoming_states();

				int cardinalita_vicinato_entrante_reference = vicinato_entrante_reference.size();
				int cardinalita_vicinato_entrante_subject = vicinato_entrante_subject.size();

				double in_similarity=0;

				if(cardinalita_vicinato_entrante_reference>0 && cardinalita_vicinato_entrante_subject>0){


					costs=(long *)malloc(cardinalita_vicinato_entrante_reference*cardinalita_vicinato_entrante_subject*sizeof(long));

					solution=new long[cardinalita_vicinato_entrante_reference];

					for(int k=0; k<cardinalita_vicinato_entrante_reference; k++)
						for(int l=0; l<cardinalita_vicinato_entrante_subject; l++)
							costs[k*cardinalita_vicinato_entrante_subject+l]=(1-tmp_similarity[vicinato_entrante_reference[k]->get_index()][vicinato_entrante_subject[l]->get_index()])/eps;

					hungarian(&costs, cardinalita_vicinato_entrante_reference, cardinalita_vicinato_entrante_subject, solution, 0);

					for(int k=0; k<cardinalita_vicinato_entrante_reference; k++){

						if(solution[k]>=0){

							std::vector<std::tuple<int, int, double> >:: iterator it ;
							bool already_modified = false;

							// Controlla se la coppia del matching ottimo da sommare al punteggio di similarità
							// è stata già tenuta in considerazione nell'ultima iterazione:
							// se è cosi allora pesca il punteggio di similarità della coppia dalla penultima iterazione

							for (it = last_modified_by_in[i][j].begin(); it != last_modified_by_in[i][j].end(); ++it)
								if (get<0>(*it) == vicinato_entrante_reference[k]->get_index() && get<1>(*it) == vicinato_entrante_subject[solution[k]]->get_index()){

									sim_to_add = get<2>(*it);

									#ifdef DEBUG
										cout<<endl<<"Confronto nodi ("<<i<<", "<<j<<"): "<<endl;
										cout<<"La coppia del vicinato entrante ("<<vicinato_entrante_reference[k]->get_index()<<", "<<vicinato_entrante_subject[solution[k]]->get_index()<<"):"<<endl;
										cout<<"è già stata considerata all'iterazione precedente."<<endl<<"Considero il punteggio della iterazione STORICA precedente a quella in cui la coppia è stata aggiunta: "<<sim_to_add<<endl;
									#endif

									already_modified = true;
									break;
								}

							// se la coppia non è già intervenuta nella passata iterazione, considera
							// il loro punteggio all' ultima iterazione
							if(!already_modified){

								sim_to_add = tmp_similarity[vicinato_entrante_reference[k]->get_index()][vicinato_entrante_subject[solution[k]]->get_index()];

								#ifdef DEBUG2
									if(iter==18){
									cout<<endl<<"Confronto nodi ("<<i<<", "<<j<<"): "<<endl;
									cout<<"La coppia del vicinato entrante ("<<vicinato_entrante_reference[k]->get_index()<<", "<<vicinato_entrante_subject[solution[k]]->get_index()<<"):"<<endl;
									cout<<"NON è stata considerata all'iterazione precedente."<<endl<<"Considero il punteggio della PRECEDENTE iterazione: "<<sim_to_add<<endl<<tmp_similarity[vicinato_entrante_reference[k]->get_index()][vicinato_entrante_subject[solution[k]]->get_index()]<<endl;
									}
								#endif

								if(iter >=1 && tmp_similarity[vicinato_entrante_reference[k]->get_index()][vicinato_entrante_subject[solution[k]]->get_index()] < 1){
									last_modified_by_in[i][j].push_back(std::make_tuple(vicinato_entrante_reference[k]->get_index(), vicinato_entrante_subject[solution[k]]->get_index(), sim_to_add));
									precision_achieved = 0;	// continua a iterare
									
									#ifdef DEBUG
										cout<<endl<<"La coppia del vicinato entrante: ("<<vicinato_entrante_reference[k]->get_index()<<", "<<vicinato_entrante_subject[solution[k]]<<")"<<endl;
										cout<<"ha un punteggio di similarità < 1. Dunque aggiungila"<<endl;
										cout<<"al vettore delle coppie modificanti per lo stato "<<i<<endl;
									#endif
										
								}

							}

							in_similarity+=sim_to_add;

							if(iter==0)
								precision_achieved=0;


							#ifdef DEBUG
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
							#endif

						}
					}	//end for

					delete solution;
					free(costs);
				}	// end if

				if(max(cardinalita_vicinato_entrante_reference,cardinalita_vicinato_entrante_subject)!=0)
					in_similarity/=(max(cardinalita_vicinato_entrante_reference,cardinalita_vicinato_entrante_subject));
				else
					in_similarity=1;

				#ifdef DEBUG
					cout << "Aggiorno sim(" << i << "," << j << "): " << node_similarity[i][j] << " -> ";
				#endif

				node_similarity[i][j]=(in_similarity+out_similarity)/2;

				#ifdef DEBUG
					cout << node_similarity[i][j] << endl;
				#endif
				

				// se uno qualunque tra gli elementi della matrice di similarità corrente
				// sottratto in valore assoluto all'elemento corrispondente della matrice
				// dell'iterazione passata, si mantiene maggiore o uguale a epsilon,
				// la condizione di terminazione non è soddisfata, continua a iterare
				/* if(abs(tmp_similarity[i][j]-node_similarity[i][j])>=eps){
					precision_achieved=0;	// continua a iterare
					if(iter > 1)
						cout << "tmp_similarity["<<i<<"]["<<j<<"]=" << tmp_similarity[i][j] << endl << "node_similarity["<<i<<"]["<<j<<"]=" << node_similarity[i][j] <<endl;
				} */

		}
		}

		#ifdef MATRICES_DEBUG
			if(iter > 1){
			std::cout<<std::endl<<"****************************************************"<<std::endl<<"Iterazione: "<<iter<<std::endl<<std::endl;
			printf("Matrice similarità corrente:\n\n");
			for(int i=0; i<num_states_; i++)
			{
				printf(" [ ");
				for(int j=0; j<num_states_subject_; j++)
					printf("%lf ", node_similarity[i][j]);
				printf("]\n");
			}
			printf("\n\nMatrice similarità precedente iterazione:\n\n");
			for(int i=0; i<num_states_; i++)
			{
				printf(" [ ");
				for(int j=0; j<num_states_subject_; j++)
					printf("%lf ", tmp_similarity[i][j]);
				printf("]\n");
			}
			}
		#endif

		iter++;
	}

	vector<vector<double> > struct_sim(num_states_+1, vector<double>(num_states_subject_));

	for(int i = 0; i<num_states_; ++i)
		for(int j = 0; j<num_states_subject_; ++j)
			struct_sim[i][j]=node_similarity[i][j];


	//Overall Dfas similarity
	/*long *r = nullptr;
    long *final_solution = nullptr;*/

    costs =(long *)malloc(num_states_*num_states_subject_*sizeof(long));
    for(int i=0; i<num_states_; i++)
	    for(int j=0; j<num_states_subject_; j++)
	        costs[i*num_states_subject_+j]=(1-struct_sim[i][j])/eps;

    solution=new long[num_states_];
    hungarian(&costs, num_states_, num_states_subject_, solution, 0);

    double similarity=0;
    int no=0;
    for(int i=0; i<num_states_; i++)
      if(solution[i]>=0)
      {
        similarity+=struct_sim[i][solution[i]];
	    no++;
      }

	if(isomorphism)
		struct_sim[num_states_][0]=similarity/no;
	else
		struct_sim[num_states_][0]=similarity/(max(num_states_, num_states_subject_));

	struct_sim[num_states_][1]=iter;


	return struct_sim;

}

template<class I>
void Dfa<I>::print_structural_similarity(vector<vector<double>> similarity_matrix) const{
	cout<<"======================================"<<endl;
	cout<<"***** NEIGHBOUR MATCHING RESULTS *****";
	printf("\nSimilarity matrix:\n\n");
	int num_states_subject_dfa = similarity_matrix[0].size();
    for(int i=0; i<this->get_num_states(); i++)
    {
        printf(" [ ");
        for(int j=0; j<num_states_subject_dfa; j++){
       		printf("%lf ", similarity_matrix[i][j]);
		}
        printf("]\n");
    }
	cout<<"--------------------------------------"<<endl;
	cout <<"Structural similarity between the ConcreteDfas: " <<similarity_matrix[this->get_num_states()][0] << endl;
	cout<<"======================================"<<endl;
}

