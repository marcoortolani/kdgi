/*
 * redbluedfa.cc
 */


#include "redbluedfa.h"

#include <iostream>
#include <fstream>
#include <sstream> //isstringstream
#include <set>


RedBlueDfa::RedBlueDfa()
:ConcreteDfa(){
	blue_states=NULL;
	red_states=NULL;
}


RedBlueDfa::RedBlueDfa(const int n_state, const vector<string> alf, const int s_state)		// Constructor 1
:ConcreteDfa(n_state, alf, s_state){

	blue_states = new vector<int>();
	red_states = new vector<int>();

}


RedBlueDfa::RedBlueDfa(const int n_state, const vector<string> alf)							// Default start state to 0
:RedBlueDfa(n_state, alf, 0){}


RedBlueDfa::RedBlueDfa(const ConcreteDfa &d1)
:ConcreteDfa(d1){

	blue_states = new vector<int>();
	red_states = new vector<int>();
}


RedBlueDfa::RedBlueDfa(const RedBlueDfa &d1)
:ConcreteDfa(d1.num_states_, d1.alphabet_, d1.start_state_, d1.get_ttable(), d1.get_accepting_states()){

	blue_states = new vector<int>;
	red_states = new vector<int>;

	copy_blue_states(d1.get_blue_states());

	copy_red_states(d1.get_red_states());

}


RedBlueDfa::~RedBlueDfa(){

	if(blue_states!=NULL){
		blue_states->clear();
		delete blue_states;
	}

	if(red_states!=NULL){
		red_states->clear();
		delete red_states;
	}
}

/*
Dfa* RedBlueDfa::to_dfa(){
	return new Dfa(this->num_states_, this->alphabet_, this->start_state_, this->get_ttable(), this->get_accepting_states());
}
*/


RedBlueDfa* RedBlueDfa::to_canonical_RedBlueDfa_from_red_states(){

	//////////////////////////////////////////////////////////////
	// - Pulisco l'automo dagli stati irragiungibili, aggiorno le transizioni

	// Conto il numero effettivo di stati finali
	int n_final_states = 0;
	for (int i = 0; i < get_num_states(); ++i)
		if (is_inside_red_states(i))
			n_final_states++;

	//TODO: Aggiungi la copia degli stati red e blue nei vectors

	// Creo un nuovo automa senza stati irragiungibili
	int count = 0;
	RedBlueDfa* finalDFA = new RedBlueDfa(n_final_states, get_alphabet());
	map<int, int> updated_transition;

	for (int i = 0; i < num_states_; ++i) {
		if (is_inside_red_states(i)) {

			for (string sym : get_alphabet()) {

				// Aggiungo lo stato al nuovo automa
				finalDFA->set_ttable_entry(count,sym,get_ttable(i,sym));

				updated_transition[i] = count;
			}
			++count;
			finalDFA->set_accepting_states_entry(i,accepting_states_[i]);
		}
	}
	updated_transition[ND] = ND;


//	if(updated_transition.size() <= 2){
//		cout << "There is only one or zero red state. Returned a copy of originale dfa."<<endl;
//		delete finalDFA;
//		return new RedBlueDfa(*this);
//	}


	bool stato_pozzo = false;
	// Aggiorno le transizioni
	for (int i = 0; i < finalDFA->get_num_states(); ++i)
		for (string sym : finalDFA->get_alphabet()) {

			if (finalDFA->get_ttable(i,sym) == ND)										// Rilevo che c'è una transizione mancante, quindi serve uno stato pozzo
				stato_pozzo = true;

			if(updated_transition.find(finalDFA->get_ttable(i,sym)) != updated_transition.end())
				finalDFA->set_ttable_entry(i, sym, updated_transition[ finalDFA->get_ttable(i,sym) ]);
			else {
				cerr << "Errore nell'aggiornamento delle stringhe"<<endl;
				exit(EXIT_FAILURE);
			}

		}

	// Stampo l'automa prima di applicare il pozzo e la minimizzazione
	//finalDFA->print_dfa_with_color("AUTOMA FINALE PREPOZZO");
	//finalDFA->print_dfa_dot("FINALEPREPOZZO", percorso.c_str());

	//finalDFA->print_dfa_dot_mapped_alphabet("FINALE_PREPOZZO", (base_path + "pulito_pre_pozzo.dot").c_str());



	//////////////////////////////////////////////////////////////
	// Controllo stato pozzo
	// - Se ci sono transizioni non definite le imposto tutte verso lo stato pozzo
	if (stato_pozzo) {
		RedBlueDfa* finalDFAPozzo = new RedBlueDfa(finalDFA->get_num_states() + 1, finalDFA->get_alphabet(), 0);

		//vector<map<string,int>> table = finalDFAPozzo->get_ttable();

		for (int i = 0; i < finalDFA->get_num_states(); ++i){
			for (string sym : finalDFA->get_alphabet()) {
				if (finalDFA->get_ttable(i,sym) == ND)
					finalDFAPozzo->set_ttable_entry(i,sym,finalDFA->get_num_states());
				else
					finalDFAPozzo->set_ttable_entry(i,sym,finalDFA->get_ttable(i,sym));
			}
			finalDFAPozzo->set_accepting_states_entry(i,finalDFA->get_accepting_states()[i]);
		}

		for (string sym : finalDFA->get_alphabet())
			finalDFAPozzo->set_ttable_entry(finalDFA->get_num_states(),sym,finalDFA->get_num_states());

		delete finalDFA;
		finalDFA = finalDFAPozzo;
	}

	return finalDFA;
}


int RedBlueDfa::get_actual_num_states()
{
	set<int> reacheable_states;

	for(int i=0; i<num_states_; ++i){
		for(string sym : alphabet_)
			if(get_ttable(i,sym) != i && get_ttable(i,sym) != ND)
				reacheable_states.insert(get_ttable(i,sym));
	}


	// Insert also the start state, colud be not reacheable from everyone by definition
	reacheable_states.insert(start_state_);

	return reacheable_states.size();
}


//TODO: RedBlueDfa* RedBlueDfa::remove_unreachable_states(){}

//TODO: void RedBlueDfa::add_sink_state(){}


void RedBlueDfa::add_blue_state(int blue_state_index)
{
	// If is not already inside, insert the new blue state
	if(!this->is_inside_blue_states(blue_state_index))
		blue_states->push_back(blue_state_index);
}

void RedBlueDfa::add_red_state(int red_state_index)
{
	// If is not already inside, insert the new blue state
	if(!this->is_inside_red_states(red_state_index))
		red_states->push_back(red_state_index);

	// Reorder the red states list. Actually there is no reason to maintain the input order.
	sort(red_states->begin(), red_states->end());
}

void RedBlueDfa::remove_blue_state(int blue_state_index)
{
	// If blue state is inside vector, remove it
	if(this->is_inside_blue_states(blue_state_index))
		blue_states->erase(remove(blue_states->begin(),blue_states->end(), blue_state_index));
}

void RedBlueDfa::remove_red_state(int red_state_index)
{
	// If red state is inside vector, remove it
	if(this->is_inside_red_states(red_state_index))
		red_states->erase(remove(red_states->begin(),red_states->end(), red_state_index));
}

vector<int>* RedBlueDfa::get_blue_states() const{
	return blue_states;
}

vector<int>* RedBlueDfa::get_red_states() const{
	return red_states;
}

int RedBlueDfa::get_num_blue_states(){
	return blue_states->size();
}

int RedBlueDfa::get_num_red_states(){
	return red_states->size();
}

bool RedBlueDfa::is_inside_blue_states(int blue_state_index)
{
	if ( find(blue_states->begin(), blue_states->end(), blue_state_index) != blue_states->end() )
	   return true;
	else
	   return false;
}


bool RedBlueDfa::is_inside_red_states(int red_state_index)
{
	if ( find(red_states->begin(), red_states->end(), red_state_index) != red_states->end() )
	   return true;
	else
	   return false;
}


void RedBlueDfa::copy_blue_states(vector<int>* new_blue_vector)
{
	if(new_blue_vector != NULL && blue_states != NULL){
		blue_states->clear();

		(*blue_states) = (*new_blue_vector);
	}
}


void RedBlueDfa::copy_red_states(vector<int>* new_red_vector)
{
	if(new_red_vector){
		red_states->clear();

		(*red_states) = (*new_red_vector);
	}
}


void RedBlueDfa::print_dfa_with_color(string title)
{
	// STAMPA IL DFA
	// Nel numero di colonne è inclusa la colonna finale di stato accettante o meno (accettante -> 1)

	cout << endl<< "--------------------------" << endl;
	cout << title << endl;
	string header = "    ";
		for(string sym : alphabet_)
			header = header + " | "+ sym;
		header = header + " - A  - C";
	cout << header << endl;

	for(int i=0; i<num_states_; ++i){
		cout << "S"<<i<<"  ";

		for(string sym : alphabet_)//+1
		{

			if(get_ttable(i,sym) == ND)				// Valori delle transizioni, o ND o il valore
				cout << " N ";
				cout << " "<< get_ttable(i,sym) <<" ";

		}

				if(accepting_states_[i] == DFA_STATE_NON_ACCEPTING )
					cout << "  / ";
				else if(accepting_states_[i] == DFA_STATE_ACCEPTING)
					cout << " Ac ";
				else if(accepting_states_[i] == DFA_STATE_REJECTING)
					cout << " Ri ";
				else
					cout << "  X ";

		if(!this->is_inside_blue_states(i) && !this->is_inside_red_states(i))
			cout << " W ";
		else if(this->is_inside_blue_states(i))
			cout << " B";
		else
			cout << " R";

		cout << endl;
	}

	cout << "--------------------------" << endl;
}


void RedBlueDfa::print_dfa_dot(string title, const char *file_path)
{
	ofstream myfile;
	myfile.open(file_path);

	string header = "digraph "+title+" {\n";
	string start_state = "__start0 [label=\"\" shape=\"none\"];\n\n";

	start_state = start_state + "rankdir=LR;\nsize=\"8,5\";\n\n";

	//States
	string states = "";
	string shape = "";
	string style="";
	string color="";
	for(int i=0; i<num_states_; ++i)
	{
		if(accepting_states_[i] == DFA_STATE_UNREACHABLE)
			continue;

		if(accepting_states_[i] == DFA_STATE_ACCEPTING){
			shape = "doublecircle";
			style = "rounded,filled";
		}
		else if(accepting_states_[i] == DFA_STATE_REJECTING){
			shape = "circle";
			style = "filled";
		} else {
			shape = "circle";
			style = "filled";
		}

		if(is_inside_red_states(i))
			color="#ff817b";
		else if(is_inside_blue_states(i))
			color="powderblue";
		else
			color="white";

		states = states + "s"+intTostring(i)+" [style=\""+style+"\", color=\"black\", fillcolor=\""+color+"\" shape=\""+shape+"\", label=\""+intTostring(i)+"\"];\n";
	}

	// Transizioni
	string transitions = "";
	for(int i=0; i<num_states_; ++i){
		for(string sym : alphabet_){
			int arrive_state = get_ttable(i,sym);
			if(arrive_state == ND)
				continue;

			transitions = transitions + "s"+intTostring(i)+" -> s"+intTostring(arrive_state)+" [label=\""+	sym+"\"];\n";
		}
	}

	string end = "__start0 -> 0;";
	string footer ="\n}";

	myfile << header << start_state << states << transitions /*<< end*/<<footer;

	myfile.close();
}

/*
void RedBlueDfa::print_dfa_dot_mapped_alphabet(string title, const char *file_path)
{
	string state_name_prefix = "q";
	ofstream myfile;
	myfile.open(file_path);

	string header = "digraph "+title+" {\n";

	string start_state = "__start0 [style = invis, shape = none, label = \"\", width = 0, height = 0];\n\n";

	start_state = start_state + "rankdir=LR;\nsize=\"8,5\";\n\n";

	string start_arrow = "";
	start_arrow = "subgraph cluster_main { \n\tgraph [pad=\".75\", ranksep=\"0.15\", nodesep=\"0.15\"];\n\t style=invis; \n\t__start0 -> s0 [penwidth=2];\n}\n";

	//States
	string states = "";
	string shape = "";
	string style="";
	string color="";
	for(int i=0; i<num_states_; ++i)
	{
		if(ttable_[i][dim_alphabet_] == DFA_STATE_UNREACHABLE)
			continue;

		if(ttable_[i][dim_alphabet_] == DFA_STATE_ACCEPTING){
			shape = "doublecircle";
			style = "rounded,filled";
		}
		else if(ttable_[i][dim_alphabet_] == DFA_STATE_REJECTING){
			shape = "circle";
			style = "filled";
		} else {
			shape = "circle";
			style = "filled";
		}

		if(is_inside_red_states(i))
			color="#ff817b";
		else if(is_inside_blue_states(i))
			color="powderblue";
		else
			color="white";

		states = states + "s"+intTostring(i)+" [style=\""+style+"\", color=\"black\", fillcolor=\""+color+"\" shape=\""+shape+"\", label=\""+state_name_prefix+intTostring(i)+"\"];\n";
	}


	// Transizioni
	string transitions = "";
	//map<string, string> label_for_transiction;					// La chiave individua una coppia di stati tra cui potrebbe esserci una transizione
																	// Il valore è la label da stampare, contenente tutti i simboli per cui avviene quella transizione

	vector< vector<string> > label_for_transiction(num_states_, vector<string>(num_states_));

	for(int i=0; i<num_states_; ++i){
		for(int j=0; j<dim_alphabet_; ++j){

			int arrive_state = ttable_[i][j];

			if(arrive_state == ND)
				continue;

			string transition_symbol = alphabet_[j];

			if(label_for_transiction[i][arrive_state].length() == 0)
				label_for_transiction[i][arrive_state] = label_for_transiction[i][arrive_state] + transition_symbol;
			else if(label_for_transiction[i][arrive_state].length() % 9 == 0)			// Inserisce ogni 7 simboli un ritorno a capo nella label
				label_for_transiction[i][arrive_state] = label_for_transiction[i][arrive_state] + "\\n" + transition_symbol;
			else
				label_for_transiction[i][arrive_state] = label_for_transiction[i][arrive_state] + "," +transition_symbol;


			// ORIGINALE un carattere - una transizione
			//transitions = transitions + "s"+intTostring(i)+" -> s"+intTostring(arrive_state)+" [label=\""+inverse_mapped_alphabet[alphabet_symbols[j]]+"\"];\n";
		}
	}


	for(int i=0; i<num_states_; ++i)
		for(int j=0; j<num_states_; ++j){
			if(label_for_transiction[i][j].compare(""))
				transitions = transitions + "s"+intTostring(i)+" -> s"+intTostring(j)+" [label=\""+label_for_transiction[i][j]+"\"];\n";
		}

	string end = "__start0 -> 0;";
	string footer ="\n}";

	myfile << header << start_state <<  states << start_arrow << transitions <<footer;

	myfile.close();
}
*/
