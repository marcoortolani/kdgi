/*
 * discrimnode.cc
 *
 *  Created on: 10 set 2018
 *      Author: marco
 */

#include "discrimnode.h"

DiscrimNode :: DiscrimNode(vector<symbol_> ph, DiscrimNode* father){
	phrase_ = ph;
	father_ = father;
	final_ = false;
	child_0_ = NULL;
	child_1_ = NULL;
};

DiscrimNode :: ~DiscrimNode(){
	if(child_0_ != NULL){
		delete child_0_;
	}
	if(child_1_ != NULL){
		delete child_1_;
	}
}

void DiscrimNode :: set_child(DiscrimNode* child, bool what_child){
	if(what_child){
		child_1_ = child;
	}
	else{
		child_0_ = child;
	}
};

DiscrimNode* DiscrimNode :: get_child(bool what_child){
	if(what_child){
		return child_1_;
	}
	else{
		return child_0_;
	}
};

void DiscrimNode :: set_father(DiscrimNode* father){
	father_ = father;
};

DiscrimNode* DiscrimNode :: get_father(){
	return father_;
};

bool DiscrimNode :: is_leaf(){
	return child_0_ == NULL && child_1_ == NULL;
};

bool DiscrimNode :: is_final(){
	return final_;
};

bool DiscrimNode :: is_block_root(){
	return !final_ && father_->final_;
}

void DiscrimNode :: set_final(){
	final_ = true;
};

bool DiscrimNode :: is_accepting(){
	DiscrimNode* father = get_father();
	if(father == NULL){
		cerr << "Error in DiscrimNode :: is accepting" << endl;
		throw 0;
	}

	DiscrimNode* grand = father->get_father();
	if(grand != NULL){
		return father->is_accepting();
	}

	if(!father->get_phrase().empty()){
		cerr << "Error in DiscrimNode :: is accepting" << endl;
		cerr << "first ancestor has non empty phrase" << endl;
		throw 0;
	}

	if(this == father->get_child(0)){
		return false;
	}

	if(this == father->get_child(1)){
		return true;
	}

	cerr << "Error in DiscrimNode :: is accepting" << endl;
	cerr << "father and son linked incorrectly" << endl;
	throw 0;
}

bool DiscrimNode :: is_accepting(vector<symbol_> phrase, bool& truth){
	if(!is_leaf()){
		cerr << "Error in DiscrimNode :: is_accepting (vector<symbol_>, bool&)" << endl;
		cerr << "the node is not a leaf" << endl;
		throw 0;
	}

	vector<symbol_> prefix = get_phrase();
	DiscrimNode* disc = this;
	//Risalgo l'albero di discriminazione fino ad arrivare alla radice
	while(disc->get_father() != NULL){
		//Compongo la frase a partire dal prefisso di questo nodo e al suffisso dell0 antenato raggiunto.
		DiscrimNode* father = disc->get_father();
		vector<symbol_> other_phrase = prefix;
		vector<symbol_> suffix = father->get_phrase();
		other_phrase.insert(other_phrase.end(), suffix.begin(), suffix.end());

		//Se le 2 frasi coincidono controllo l'informazione cercata in base alla posizione del figlio rispetto al padre.
		if(other_phrase == phrase){
			if(disc == father->get_child(0)){
				truth = false;
			}
			else if(disc == father->get_child(1)){
				truth = true;
			}
			else{
				cerr << "Error in DiscrimNode :: is_accepting (vector<symbol_>, bool&)" << endl;
				throw 0;
			}
			return true;
		}
		disc = father;
	}

	return false;
}

bool DiscrimNode :: is_accepting(vector <symbol_> prefix, vector<symbol_> phrase, bool& truth){
	DiscrimNode* disc = this;
	DiscrimNode* father = get_father();
	//Scorro tutti gli antenati del nodo
	while(father != NULL){
		//Se la combinazione tra il prefisso passato per argomento
		vector<symbol_> other_phrase = prefix;
		//e il suffisso preso dal nodo antenato
		vector<symbol_> suffix = father->get_phrase();
		other_phrase.insert(other_phrase.end(), suffix.begin(), suffix.end());
		//è uguale alla frase passata per argomento
		if(other_phrase == phrase){
			//Restituisco l'accettanza o meno del nodo figlio rispetto al padre
			//ovvero se il nodo a cui siamo giunti è un figlio-0 o un figlio-1.
			if(disc == father->get_child(0)){
				truth = false;
			}
			else if(disc == father->get_child(1)){
				truth = true;
			}
			else{
				cerr << "Error in DiscrimNode :: is_accepting (vector<symbol_>, vector<symbol_>, bool&)" << endl;
				throw 0;
			}
			//(in ogni caso abbiamo ottenuto la risposta cercata).
			return true;
		}
		disc = father;
		father = disc->get_father();
	}

	//Se siamo giunti a questo punto allora abbiamo raggiunto il nodo radice senza trovare la frase.
	//La frase cercata non è reperibile da questo nodo.
	return false;
}

void DiscrimNode :: set_phrase(vector <symbol_> ph){
	phrase_ = ph;
};

vector<symbol_> DiscrimNode :: get_phrase(){
	return phrase_;
};

DiscrimNode* DiscrimNode :: get_block_root(){

	if(final_){
		cerr << "Error in DiscrimNode :: get_block_root" << endl;
		cerr << "a final node is not part of any block" << endl;
		throw 0;
	}

	//Si sale fino a che non si trova un nodo NON finale, il cui padre sia invece finale.
	DiscrimNode* d = this;
	for(;!d->get_father() == NULL && !d->get_father()->is_final(); d = d->get_father());

	return d;
}

DiscrimNode* DiscrimNode :: get_lowest_common_discriminator(DiscrimNode* other_node){
	if(other_node == NULL){
		cerr << "Error in DiscrimNode :: get_lowest_common_discriminator" << endl;
		cerr << "other_node can't be NULL" << endl;
		throw 0;
	}

	if(this == other_node){
		return this;
	}

	//Conservo i nodi di partenza come propri antenati
	DiscrimNode* current_ancestor = this;
	DiscrimNode* other_current_ancestor = other_node;
	set<DiscrimNode*> this_ancestors;
	set<DiscrimNode*> other_ancestors;
	//Ad ogni passo aggiungo un antenato precedente ad entrambi gli insiemi
	while(current_ancestor != NULL && other_current_ancestor != NULL){
		//controllo se sono uguali
		if(current_ancestor == other_current_ancestor){
			return current_ancestor;
		}
		//controllo se uno appartiene all'insieme degli antenati dell'altro
		if(other_ancestors.find(current_ancestor) != other_ancestors.end()){
			return current_ancestor;
		}
		//e viceversa.
		if(this_ancestors.find(other_current_ancestor) != this_ancestors.end()){
			return other_current_ancestor;
		}

		this_ancestors.insert(current_ancestor);
		other_ancestors.insert(other_current_ancestor);

		current_ancestor = current_ancestor->get_father();
		other_current_ancestor = other_current_ancestor->get_father();
	}

	//Se terminano prima gli antenati di uno (nel caso siano a profondità differenti)
	//controllo i restanti antenati del secondo
	while(current_ancestor != NULL){
		if(other_ancestors.find(current_ancestor) != other_ancestors.end()){
			return current_ancestor;
		}

		current_ancestor = current_ancestor->get_father();
	}

	//e viceversa.
	while(other_current_ancestor != NULL){
		if(this_ancestors.find(other_current_ancestor) != this_ancestors.end()){
			return other_current_ancestor;
		}

		other_current_ancestor = other_current_ancestor->get_father();
	}

	//Se 2 nodi appartengono allo stesso albero devono avere per forza un antenato comune.
	//Quindi o i 2 nodi passati appartengono ad alberi diversi o l'albero è stato costruito male.
	cerr << "Error in DiscrimNode :: get_lowest_common_discriminator" << endl;
	cerr << "could not find a common ancestor between ";
	if(this->is_leaf()){
		cerr << " leaf ";
	}
	else{
		cerr << " node ";
	}
	for(symbol_ s : this->get_phrase()){
		cerr << s;
	}
	if(other_node->is_leaf()){
		cerr << " and leaf ";
	}
	else{
		cerr << " and node ";
	}
	for(symbol_ s : other_node->get_phrase()){
		cerr << s;
	}
	cerr << endl;
	throw 0;
}

vector<DiscrimNode*> DiscrimNode :: get_all_leaves(){
	vector<DiscrimNode*> result;
	list<DiscrimNode*> next_nodes = {this};

	//Esploriamo il sottoalbero di cui questo nodo è radice
	//con una ricerca in ampiezza
	while(!next_nodes.empty()){
		DiscrimNode* node = next_nodes.back();
		next_nodes.pop_back();

		bool leaf = true;
		//Verifichiamo se il nodo ha un figlio-0 o un figlio-1 o entrambi
		//(e quindi se il nodo è una foglia).
		for(int i=0; i < 2; ++i){
			DiscrimNode* child = node->get_child(i);
			//Se esiste un figlio lo aggiungiamo ai nodi da esplorare
			if(child != NULL){
				leaf = false;
				next_nodes.push_front(child);
			}
		}

		//Se il nodo non ha figli va aggiunto al vettore risultato.
		if(leaf){
			result.push_back(node);
		}
	}

	return result;
}

void DiscrimNode :: destroy_all_but_leaves(){
	bool leaf = true;
	//Per ogni figlio esistente richiamo la stessa funzione.
	if(child_0_ != NULL){
		child_0_->destroy_all_but_leaves();
		leaf = false;
	}
	if(child_1_ != NULL){
		child_1_->destroy_all_but_leaves();
		leaf = false;
	}

	//Quindi se questo nodo non era una foglia procedo a distruggerlo.
	if(!leaf){
		//Impostare i puntatori a NULL è necessario perchè altrimenti i figli sarebbero distrutti a loro volta,
		//ma arrivati a questo punto del codice i casi sono 2:
		//o i figli si sono già distrutti perchè hanno richiamato la stessa funzione (nel qual caso una seconda delete darebbe errore)
		//o i figli sono foglie e dunque non vanno distrutti.
		child_0_ = NULL;
		child_1_ = NULL;
		delete this;
	}
}

string DiscrimNode :: get_ancestors_string(){
	string ancestors = "";
	DiscrimNode* father = this;
	//Per ognuno degli antenati (compreso questo nodo)
	while(father != NULL){
		//aggiungo la stringa relativa al nodo corrente
		for(symbol_ s : father->get_phrase())
			ancestors += s;

		DiscrimNode* temp = father->get_father();
		//aggiungo 0 o 1 a seconda se il nodo corrente è un figlio-0 o un figlio-1
		if(temp != NULL)
			if(temp->get_child(0) == father)
				ancestors += "0";
			else
				ancestors += "1";

		father = temp;
	}

	return ancestors;
}
