#include "angluindfa.h"

void AngluinDfa::clear_new_tables(){
	newS.clear();
	newSa.clear();
	newE.clear();
	new_look_up_s.clear();
	new_look_up_sa.clear();
}

bool AngluinDfa::correct_order(vector <int> state1, vector <int> state2){
	if(state1.size() != state2.size()){
		cerr << "Error in AngluinDfa :: correct_order" << endl;
		throw 0;
	}

	for( int i=state1.size()-1; i>=0; i--){
		if(state1[i] == 1 && state2[i] == 0){
			return false;
		}
		if(state1[i] == 0 && state2[i] == 1){
			return true;
		}
	}
	return true;
}

int AngluinDfa::equal_states(vector<int> state1, vector<int> state2) const{

	if(state1.size() != state2.size()){
		cerr << "Error in equal_states" << endl;
		throw 0;
	}

	for(unsigned int i=0; i< state1.size(); i++){
		if(state1[i] != state2[i]){
			return i;
		}
	}

	return -1;
}

int AngluinDfa::get_index(list <list<string>> S_Sa_E, list <string> s) const {

    auto it = S_Sa_E.begin();
    int index = 0;
    while(it != S_Sa_E.end()){
		if(*it == s){
    		return index;
    	}
    	++it;
    	++index;
    }
    return -1;
}

int AngluinDfa::contain_phrase(list<string> str){

	auto it_look_up = look_up_s.begin();
	auto it = S.begin();

	//cerchiamo prima un prefisso valido tra gli stati di S
	while(it != S.end()){
		list <string> str_concat = *it;
		str_concat.remove("");

		//Se lo stato è rappresentato da una phrase più lunga della parola allora non potrà sicuramente essere un prefisso.
		//Si passa allo stato successivo
		if(str_concat.size() > str.size()){
			++it;
			++it_look_up;
			continue;
		}

		auto it_tmp = str.begin();
		bool valid_prefix = true;
		//Se lo stato è identificato da una phrase più breve della stringa ricercata allora potrebbe essere un prefisso valido,
		//per cui vengono paragonati tutti i simboli fino a che se ne trovi uno sbagliato, o si arrivi alla fine del prefisso
		//che in questo secondo caso è valido.
		for(symbol_ sym : str_concat){
			if(sym != *it_tmp){
				valid_prefix = false;
				break;
			}
			++it_tmp;
		}

		//Se il prefisso non è valido si passa allo stato di S successivo.
		if(!valid_prefix){
			++it;
			++it_look_up;
			continue;
		}

		//Per ogni suffisso contenuto in E si verifica se la parola ottenuta combinando prefisso di S con suffisso
		//è quella ricercata, nel qual caso viene restituito il valore memorizzato per quella combinazione prefisso suffisso.
		for(unsigned int j=0; j < E.size(); j++){
			str_concat = *it;
			str_concat.insert(str_concat.end(), E[j].begin(), E[j].end());
			str_concat.remove("");
			if(str == str_concat){
				return (*it_look_up)[j];
			}
		}
		++it;
		++it_look_up;
	}

	//cerchiamo adesso un prefisso valido in Sa (l'intero processo viene ripetuto identicamente)
	it_look_up = look_up_sa.begin();
	it = Sa.begin();

	while(it != Sa.end()){
		list <string> str_concat = *it;
		str_concat.remove("");

		if(str_concat.size() > str.size()){
			++it;
			++it_look_up;
			continue;
		}

		auto it_tmp = str.begin();
		bool valid_prefix = true;
		for(symbol_ sym : str_concat){
			if(sym != *it_tmp){
				valid_prefix = false;
			}
			++it_tmp;
		}

		if(!valid_prefix){
			++it;
			++it_look_up;
			continue;
		}

		for(unsigned int j=0; j < E.size(); j++){
			str_concat = (*it);
			str_concat.insert(str_concat.end(), E[j].begin(), E[j].end());
			str_concat.remove("");
			if(str == str_concat){
				return (*it_look_up)[j];
			}
		}
		++it;
		++it_look_up;
	}

	//Se non venisse trovata alcuna coppia prefisso (da S o Sa) - suffisso (da E) che corrispondesse alla parola cercata
	//viene restituito -1 per indicare la non conoscenza della parola.
	return -1;
}

void AngluinDfa::insert_row(string table, list<string> sa, vector<int> queries){

	//Per inserire una nuova riga il numero di queries deve essere pari al numero di colonne di S ed Sa,
	//ovvero la dimensione di E.
	if(queries.size() != E.size()){
		cerr << "Error in AngluinDfa: queries and E size dimension must agree" << endl;
		throw 0;
	}

	list <list<string>>* S_Sa;
	list <vector<int>>* look_up_s_sa;

	//La nuova riga verrà inserita in S o in Sa a seconda del valore di table.
	if(table == "S"){
		S_Sa = &S;
		look_up_s_sa = &look_up_s;
	}
	else{
		S_Sa = &Sa;
		look_up_s_sa = &look_up_sa;
	}

	//Scorro S (o Sa) finchè non viene valutato che quella raggiunta è la posizione dove inserire la nuova riga.
	bool added_row = false;
	auto it = S_Sa->begin();
	auto it_look_up = look_up_s_sa->begin();
	while(it != S_Sa->end() && !added_row){
		if(correct_order(queries, *it_look_up)){
			S_Sa->emplace(it, sa);
			look_up_s_sa->emplace(it_look_up, queries);
			added_row = true;
		}

		if(it != S_Sa->end() && it_look_up != look_up_s_sa->end()){
			++it;
			++it_look_up;
		}
		else{
			cerr << "Error in insert_row" << endl;
			throw 0;
		}
	}

	//Se si è arrivati al termine di S (Sa) senza aver inserito la nuova riga allora questa va aggiunta alla fine.
	if(!added_row){
		look_up_s_sa->push_back(queries);
		S_Sa->push_back(sa);
	}
}

void AngluinDfa::delete_row(list<string> sa){
	//Trovo l'indice di Sa che corrisponde alla stringa sa.
	int index = get_index(Sa, sa);
	if(index != -1){
		//Se esiste cancello quella riga.
		auto it_look_up = look_up_sa.begin();
		advance(it_look_up, index);
		Sa.remove(sa);
		look_up_sa.erase(it_look_up);
	}
	/*index = get_index(newSa, sa);
	if(index != -1){
		auto it_look_up = new_look_up_sa.begin();
		advance(it_look_up, index);
		newSa.remove(sa);
		new_look_up_sa.erase(it_look_up);
	}*/
	return;
}

AngluinDfa::AngluinDfa(vector <string> ab, map <list<string>, bool>* first_queries) : Dfa(){

	//Per iniziare l'algoritmo devono essere note le query relative ad ogni simbolo dell'alfabeto
	//e quella relativa a epsilon.
	if(first_queries->size() != ab.size()+1){
		cerr << "Error in AngluinDfa Constructor" << endl;
		cerr << "mem_query_res size must be equal to alphabet size + 1" << endl;
		throw 0;
	}

	set_alphabet(ab);

	//Inizializzo la tabella S con il solo prefisso epsilon.
	list<string> epsilon = list<string>(1, "");
	S.clear();
	S.push_back(epsilon);

	bool accepting_epsilon = first_queries->find(epsilon)->second;
	accepting_epsilon ? look_up_s = list <vector<int>>(1, vector<int>(1,1)) : look_up_s = list <vector<int>>(1, vector<int>(1,0));

	//Inizializzo la tabella Sa con ogni simbolo dell'alfabeto come prefisso.
	Sa.clear();
	for(symbol_ sym : ab){
		auto it = first_queries->find(list<string>(1,sym));
		if(it != first_queries->end()){
			if(it->second){
				Sa.push_back(list<string>(1,sym));
				look_up_sa.push_back(vector<int>(1,1));
			}
			else{
				Sa.push_front(list<string>(1,sym));
				look_up_sa.push_front(vector<int>(1,0));
			}
		}
		else{
			cerr << "Error in AngluinDfa Constructor" << endl;
			cerr << "couldn't find any query for symbol " << sym << endl;
			throw 0;
		}
	}

	//Inizializzo E con il solo suffisso epsilon
	E.clear();
	E.push_back(epsilon);
}

bool AngluinDfa::get_inconsistent_phrases(map <list<string>, bool>* strings_to_verify){

	//Se il numero di righe è inferiore a 2 S non può essere inconsistente.
    int size = look_up_s.size();
    if(size < 2){
        return true;
    }

    //Per gestire le tabelle ho fatto ricorso a liste, data la continua necessità di inserimenti nel mezzo.
    //Purtroppo in questa funzione è necessario l'accesso randomico.
    //Piuttosto che scorrere ogni volta tutta la lista alla ricerca dell'elemento (per ogni elemento),
    //ho preferito copiare l'intera lista (una sola volta) in un vettore e poi usare quello per l'accesso.
    vector <vector<int>> vec_look_up_s;
    vec_look_up_s.insert(vec_look_up_s.end(), look_up_s.begin(), look_up_s.end());
    vector <vector<int>> vec_look_up_sa;
	vec_look_up_sa.insert(vec_look_up_sa.end(), look_up_sa.begin(), look_up_sa.end());

	//Scorro la tabella S 2 righe alla volta (successive).
    auto it = S.begin();
    auto it1 = it;
    ++it1;
    auto it_look_up = look_up_s.begin();
    auto it_look_up1 = it_look_up;
    ++it_look_up1;
    while(it1 != S.end()){
    	//Se queste corrispondono allo stesso stato
        if(*it_look_up == *it_look_up1){
        	//Per ogni simbolo controllo le 2 righe successive e
        	//verifico che corrispondano allo stesso stato a loro volta.
            for(symbol_ sym : get_alphabet()){

                int index1, index2;
                vector <int> state1, state2;

                list <string> str1 = *it;
                str1.push_back(sym);
                str1.remove("");
                list <string> str2 = *it1;
				str2.push_back(sym);
				str2.remove("");

				//Cerco la prima riga successiva in S e in Sa.
                index1 = get_index(S, str1);
                if(index1 != -1){
                    state1 = vec_look_up_s[index1];
                }
                else{
                    index1 = get_index(Sa, str1);
                    state1 = vec_look_up_sa[index1];
                }

                //Cerco la seconda riga successiva in S e in Sa.
                index2 = get_index(S, str2);
                if(index2 != -1){
                	state2 = vec_look_up_s[index2];
                }
                else{
                    index2 = get_index(Sa, str2);
                    state2 = vec_look_up_sa[index2];
                }

                //Anche se non dovessero corrispondere queste righe devono comunque essere presenti
                //in una delle 2 tabelle.
                if(index1 == -1 || index2 == -1){
                    cerr << "Error in function get_inconsistent_string" << endl;
                    cerr << "string1 :";
                    for(auto j = str1.begin(); j != str1.end(); ++j){
                    	cerr << *j;
                    }
                    cerr << endl;
            		cerr << "string2 :";
            		for(auto j = str2.begin(); j != str2.end(); ++j){
						cerr << *j;
					}
            		cerr << endl;
            		throw 0;
                }

                //Verifico quale sia (se esiste) il primo suffisso che distingue i 2 prefissi (con l'aggiunta della transizione).
                int indexE = equal_states(state1,state2);
                if(indexE != -1){

                    list <string> new_suffix(1, sym);
                    new_suffix.insert(new_suffix.end(), E[indexE].begin(), E[indexE].end());
                    new_suffix.remove("");

                    //Se esiste allora aggiorno le tabelle "ausiliarie" perchè permettano
                    //l'aggiunta di una nuova colonna alle tabelle S ed Sa dopo che il learner avrà
                    //effettuato le queries necessarie.

                    //newE dovrà contenere solamente il nuovo suffisso
                    clear_new_tables();
                    newE = vector<list<string>>(1, new_suffix);

                    //new_look_up dovrà contenere tante righe quante look_up ma una sola colonna.
                    for(auto i1 = S.begin(); i1 != S.end(); ++i1){
                    	list <string> str(*i1);
                    	str.insert(str.end(), new_suffix.begin(), new_suffix.end());
                    	str.remove("");
                    	//Se è possibile inferire alcune query dalle tabelle già presenti
                    	//queste non verranno richieste all'oracolo.
                    	int contain = contain_phrase(str);
                    	new_look_up_s.push_back(vector<int>(1,contain));
                    	if(contain == -1){
                    		(*strings_to_verify)[str];
                    	}
                    }

                    //new_look_up_sa dovrà avere tante righe quante look_up_sa e una sola colonna.
                    //Il ragionamento è lo stesso di quello sopra.
                    for(auto i1 = Sa.begin(); i1 != Sa.end(); ++i1){
						list <string> str(*i1);
						str.insert(str.end(), new_suffix.begin(), new_suffix.end());
						str.remove("");

						int contain = contain_phrase(str);
						new_look_up_sa.push_back(vector<int>(1,contain));
						if(contain == -1){
							(*strings_to_verify)[str];
						}
					}
                    return false;
                }
            }
        }

        if(it != S.end() && it1 != S.end() && it_look_up != look_up_s.end() && it_look_up1 != look_up_s.end()){
        	++it;
			++it1;
			++it_look_up;
			++it_look_up1;
        }
        else{
        	cerr << "Error in get_inconsistent_phrases" << endl;
        	throw 0;
        }
    }
    return true;
}

bool AngluinDfa::get_open_phrases(map <list<string>, bool>* strings_to_verify){
	auto it = Sa.begin();
	//Cerco tra gli stati codificati in look_up_sa uno stato non presente in look_up_s e che quindi rende aperto il Dfa.
	for(auto i : look_up_sa){
		bool state_not_found = true;
        for(auto j : look_up_s){
        	//equal_states restituisce l'indice del primo suffisso di E per cui le funzioni di output dei 2 stati sono differenti,
        	//-1 se i due stati non sono differenti per alcun suffisso.
            if(equal_states(i, j) == -1){
                state_not_found = false;
                break;
            }
        }

        //Se lo stato non viene trovato e dunque il Dfa è aperto copio lo stato in S e aggiungo gli a-successori (per ogni simbolo a dell'alfabeto)
        //ad Sa (o meglio preparo le tabelle newS e newSa visto che le informazioni saranno richieste all'oracolo).
        if(state_not_found){
        	clear_new_tables();
        	newS.push_back(*it);
        	new_look_up_s = list <vector<int>>(1, static_cast<vector<int>>(i));
        	/*new_look_up_s = list <vector<int>>(1, vector<int>(E.size(), 0));

        	for(unsigned int j=0; j<E.size(); j++){
        		list <string> str(*(newS.begin()));
        		str.insert(str.end(), E[j].begin(), E[j].end());
        		str.remove("");
        		int contained = contain_phrase(str);
        		(*new_look_up_s.begin())[j] = contained;
        		if(contained == -1){
        			(*strings_to_verify)[str];
        		}
        	}*/

        	for(unsigned int j=0; j<alphabet_.size(); j++){
				list<string> str = *it;
				str.push_back(alphabet_[j]);

				vector<int> row(E.size(), 0);
				for(unsigned int k=0; k<E.size(); k++){
					list<string> stre = str;
					stre.insert(stre.end(), E[k].begin(), E[k].end());
					stre.remove("");
					int contained = contain_phrase(stre);
					row[k] = contained;
					if(contained == -1){
						(*strings_to_verify)[stre];
					}
				}

				newSa.push_back(str);
				new_look_up_sa.push_back(row);
			}
            return false;
        }

        if(it != Sa.end()){
        	++it;
        }
        else{
        	cerr << "Error in AngluinDfa :: get_open_phrases" << endl;
			throw 0;
        }
    }
    return true;
}

void AngluinDfa::get_phrases_from_counterexample(map <list<string>, bool>* strings_to_verify, vector<string> str){
	clear_new_tables();

	//Se viene fornito un controesempio questo deve essere aggiunto insieme ad ogni suo prefisso ad S (se queste stringhe non sono già presenti)
	list<symbol_> substr;
	for(symbol_ sym : str){
		substr.push_back(sym);

		if(get_index(S, substr) == -1){

			newS.push_back(substr);
			new_look_up_s.push_back(vector<int>(E.size(), 0));

			for(unsigned int j=0; j<E.size(); j++){
				list <string> str1 = substr;
				str1.insert(str1.end(), E[j].begin(), E[j].end());
				str1.remove("");
				int contain = contain_phrase(str1);
				new_look_up_s.back()[j] = contain;
				if(contain == -1){
					(*strings_to_verify)[str1];
				}
			}

			//Per ogni prefisso aggiunto a newS(e new_look_up) bisogna aggiungere gli a-successori a newSa(e new_look_up_s)
			for(unsigned int j=0; j<alphabet_.size(); j++){
				list<string> stra = substr;
				stra.push_back(alphabet_[j]);
				newSa.push_back(stra);
				new_look_up_sa.push_back(vector<int>(E.size(), 0));
				for(unsigned int k=0; k<E.size(); k++){
					list<string> str1 = stra;
					str1.insert(str1.end(), E[k].begin(), E[k].end());
					str1.remove("");
					int contain = contain_phrase(str1);
					new_look_up_sa.back()[k] = contain;
					if(contain == -1){
						(*strings_to_verify)[str1];
					}
				}
			}
		}
	}
}

void AngluinDfa::extend_column(map <list<string>, bool>* mem_query_res){
	vector<vector<int>> sorted_int;
	vector<list<string>> sorted_string;
	auto it = S.begin();
	auto it_look_up = look_up_s.begin();
	auto it_new_look_up = new_look_up_s.begin();

	//Per ogni riga di look_up_s bisogna aggiungere un elemento
	while(it_new_look_up != new_look_up_s.end()){
		//Se il risultato era già noto è stato salvato in new_look_up_s e si recupera da li
		if((*it_new_look_up)[0] != -1){
			(*it_look_up).push_back((*it_new_look_up)[0]);
		}
		//Altrimenti deve essere tra le query fatte all'oracolo.
		else{
			list<string> str = *it;
			str.insert(str.end(), newE[0].begin(), newE[0].end());
			str.remove("");
			map<list<string>, bool> :: iterator pair = mem_query_res->find(str);
			if(pair != mem_query_res->end()){
				(*it_look_up).push_back(pair->second);
			}
			else{
				cerr << "Error in extend_column" << endl;
				throw 0;
			}
		}

		//A questo punto a seconda del risultato bisogna riordinare le righe:
		//Quelle con nuovo risultato pari a 1 devono essere inserite alla fine della tabella,
		//quindi vengono spostate temporaneamente in tabelle ausiliarie.
		if((*it_look_up).back() == 1){
			sorted_int.push_back(*it_look_up);
			sorted_string.push_back(*it);

			auto t = it;
			auto t_look_up = it_look_up;

			if(it != S.end() && it_look_up != look_up_s.end()){
				++it;
				++it_look_up;
			}
			else{
				cerr << "Error in extend_column" << endl;
				throw 0;
			}

			//ed eliminate da S e look_up_s
			S.erase(t);
			look_up_s.erase(t_look_up);
		}
		//Se il nuovo risutato è 0 non bisogna spostare la riga, perchè rimarrà sul "fondo" della tabella
		else{
			if(it != S.end() && it_look_up != look_up_s.end()){
				++it;
				++it_look_up;
			}
			else{
				cerr << "Error in extend_column" << endl;
				throw 0;
			}
		}

		++it_new_look_up;
	}

	//Inserisco le righe che hanno 1 come ultimo elemento alla fine delle tabelle
	S.insert(S.end(), sorted_string.begin(), sorted_string.end());
	look_up_s.insert(look_up_s.end(), sorted_int.begin(), sorted_int.end());

	//Ripeto l'algoritmo per Sa e look_up_sa.
	sorted_string.clear();
	sorted_int.clear();

	it = Sa.begin();
	it_look_up = look_up_sa.begin();
	it_new_look_up = new_look_up_sa.begin();

	while(it_new_look_up != new_look_up_sa.end()){

		if((*it_new_look_up)[0] != -1){
			(*it_look_up).push_back((*it_new_look_up)[0]);
		}
		else{
			list<string> str = *it;
			str.insert(str.end(), newE[0].begin(), newE[0].end());
			str.remove("");
			map<list<string>, bool> :: iterator pair = mem_query_res->find(str);
			if(pair != mem_query_res->end()){
				(*it_look_up).push_back(pair->second);
			}
			else{
				cerr << "Error in extend_column" << endl;
				throw 0;
			}
		}

		if((*it_look_up).back() == 1){
			sorted_int.push_back(*it_look_up);
			sorted_string.push_back(*it);

			auto t = it;
			auto t_look_up = it_look_up;

			if(it != S.end() && it_look_up != look_up_s.end()){
				++it;
				++it_look_up;
			}
			else{
				cerr << "Error in extend_column" << endl;
				throw 0;
			}

			Sa.erase(t);
			look_up_sa.erase(t_look_up);
		}
		else{
			if(it != Sa.end() && it_look_up != look_up_sa.end()){
				++it;
				++it_look_up;
			}
			else{
				cerr << "Error in extend_column" << endl;
				throw 0;
			}
		}

		++it_new_look_up;
	}

	Sa.insert(Sa.end(), sorted_string.begin(), sorted_string.end());
	look_up_sa.insert(look_up_sa.end(), sorted_int.begin(), sorted_int.end());

	E.push_back(newE[0]);

	clear_new_tables();
}

void AngluinDfa::extend_rows(map <list<string>, bool>* mem_query_res){

	//Scorro lungo righe di newS
    auto it = newS.begin();
    for(auto it_look_up = new_look_up_s.begin(); it_look_up != new_look_up_s.end(); ++it_look_up){
    	auto itE = E.begin();
    	//Scorro lungo gli elementi della riga attuale
    	for(auto j = it_look_up->begin(); j != it_look_up->end(); ++j){
    		//Se l'elemento ha valore -1 allora la coppia suffisso-prefisso non era nota prima che venisse chiesto all'oracolo
    		//dunque aggiorno quell'elemento (0 o 1).
    		if(*j == -1){
    			list<symbol_> str = *it;
    			str.insert(str.end(), itE->begin(), itE->end());
    			str.remove("");
    			auto pair = mem_query_res->find(str);
    			if(pair != mem_query_res->end()){
    				*j = pair->second;
    			}
    			else{
    				cerr << "Error in extend_rows" << endl;
    				throw 0;
    			}
    		}
    		if(itE != E.end()){
    			++itE;
    		}
    		else{
    			cerr << "Error in extend_rows" << endl;
    		}
    	}
    	if(it != newS.end()){
    		++it;
    	}
    	else{
    		cerr << "Error in extend_rows" << endl;
    		throw 0;
    	}
    }

    //Scorro lungo newS(e new_look_up_s)
    it = newS.begin();
    auto it_look_up = new_look_up_sa.begin();
	it_look_up = new_look_up_s.begin();
	while(it != newS.end()){
		//Cancello la riga da Sa(e look_up_sa) se esiste
		delete_row(*it);
		//Inserisco la riga in S(e look_up_s) nella posizione corretta secondo l'ordinamento scelto.
		insert_row("S", *it, *it_look_up);

		++it;
		if(it_look_up != new_look_up_sa.end()){
			++it_look_up;
		}
		else{
			cerr << "Error in extend rows" << endl;
			throw 0;
		}
	}

    //Ripeto il procedimento per Sa(e look_up_sa)
    it = newSa.begin();
    for(auto i = new_look_up_sa.begin(); i != new_look_up_sa.end(); ++i){
    	auto itE = E.begin();
		for(auto j = i->begin(); j != i->end(); ++j){
			if(*j == -1){
				list<string> str = *it;
				str.insert(str.end(), itE->begin(), itE->end());
				str.remove("");
				auto pair = mem_query_res->find(str);

				if(pair != mem_query_res->end()){
					*j = pair->second;
				}
				else{
					cerr << "Error in extend_row 2" << endl;
					throw 0;
				}
			}
			if(itE != E.end()){
				++itE;
			}
			else{
				cerr << "Error in extend_rows" << endl;
			}
		}
		if(it != newS.end()){
			++it;
		}
		else{
			cerr << "Error in extend_rows" << endl;
			throw 0;
		}
	}

    it = newSa.begin();
    it_look_up = new_look_up_sa.begin();
    while(it != newSa.end()){
		insert_row("Sa", *it, *it_look_up);

		++it;
		if(it_look_up != new_look_up_sa.end()){
			++it_look_up;
		}
		else{
			cerr << "Error in extend rows" << endl;
			throw 0;
		}
	}

   clear_new_tables();
}

bool AngluinDfa::membership_query(vector<string> phrase) const {
	DfaState state = (state_table_[0]);
	DfaState* state_p = &state;
	for(symbol_ sym : phrase){
		state_p = state_p->next(sym);
	}
	return state_p->is_accepting();
}

ConcreteDfa* AngluinDfa::gen_concrete_dfa() const{

    if(look_up_s.size() == 0){
        return new ConcreteDfa(0, alphabet_);
    }

    vector <vector<int>> states;
    vector <int> indices;
    int start_state;

    int i = 0;
    auto it = S.begin();
    auto it_look_up = look_up_s.begin();
    while(it != S.end()){
        if(states.empty() || equal_states(*it_look_up, states[states.size()-1]) != -1){
            states.push_back(*it_look_up);
            indices.push_back(i);
        }

        if(it->size() == 1 && it->front() == ""){
            start_state = states.size()-1;
        }

        if(it != S.end() && it_look_up != look_up_s.end()){
        	++it;
        	++it_look_up;
        	++i;
        }
    }

    ConcreteDfa* dfa_res = new ConcreteDfa(states.size(), alphabet_, start_state);

    //used for performance reasons: random access is needed.
    vector <list<string>> vecS;
    vecS.insert(vecS.end(), S.begin(), S.end());
    vector <vector<int>> vec_look_up_s;
    vec_look_up_s.insert(vec_look_up_s.end(), look_up_s.begin(), look_up_s.end());
    vector <vector<int>> vec_look_up_sa;
    vec_look_up_sa.insert(vec_look_up_sa.end(), look_up_sa.begin(), look_up_sa.end());
    //less operations more memory.

    for(unsigned int i=0; i<states.size(); i++){

    	for(unsigned int j=0; j<alphabet_.size(); j++){

    		list <string> str = vecS[indices[i]];
            str.remove("");
            str.push_back(alphabet_[j]);
            int index = get_index(S, str);
            vector <int> next_val;

            if(index != -1){
            	next_val = vec_look_up_s[index];
            }
            else{
                index = get_index(Sa, str);

                if(index != -1){
                	next_val = vec_look_up_sa[index];
                }
                else{
                    cerr << "Error in gen_concrete_dfa" << endl;
                    throw 0;
                }
            }

            bool found_next_state = false;
            for(unsigned int k=0; k<states.size() && !found_next_state; k++){
                if(equal_states(next_val, states[k]) == -1){
                	dfa_res->set_ttable_entry(i, alphabet_[j], k);
                    found_next_state = true;
                }
            }
        }

        if(states[i][0] == 1){
        	dfa_res->set_accepting_state(i);
        }
        else{
        	dfa_res->set_rejecting_state(i);
        }
    }

    return dfa_res;
}

void AngluinDfa::print(){

	cout << "----------" << endl;
	cout << "S:" << S.size() << endl;
	cout << "----------" << endl;

	for(auto i = E.begin(); i != E.end(); ++i){
		cout << "\t\"";
		for(auto j = i->begin(); j != i->end(); ++j){
			cout << *j;
		}
		cout << "\"";
	}
	cout  << endl;

	for(unsigned int i=0; i<S.size(); i++){

		auto it = S.begin();
		advance(it, i);

		cout << "\"";
		for(auto j = it->begin(); j != it->end(); ++j){
			cout << *j;
		}
		cout  << "\"";

		auto it_look_up = look_up_s.begin();
		advance(it_look_up, i);
		vector <int> state = *it_look_up;
		for(unsigned int k=0; k<state.size(); k++){
			cout << "\t " << state[k];
		}
		cout << endl;
	}

	cout << "----------" << endl;
	cout << "Sa:" << Sa.size() << endl;
	cout << "----------" << endl;

	for(auto i = E.begin(); i != E.end(); ++i){
		cout << "\t\"";
		for(auto j = i->begin(); j != i->end(); ++j){
			cout << *j;
		}
		cout << "\"";
	}
	cout << endl;

	for(unsigned int i=0; i<Sa.size(); i++){

		auto it = Sa.begin();
		advance(it, i);

		cout << "\"";
		for(auto j = it->begin(); j != it->end(); ++j){
			cout << *j;
		}
		cout  << "\"";

		auto it_look_up = look_up_sa.begin();
		advance(it_look_up, i);
		vector <int> state = *it_look_up;
		for(unsigned int k=0; k<state.size(); k++){
			cout << "\t " << state[k];
		}
		cout << endl;
	}
}

void AngluinDfa::print_new_tables(){
	cout << "newS" << endl;
	for(auto i = newS.begin(); i != newS.end(); ++i){
		for(auto j = i->begin(); j != i->end(); ++j){
			cout << *j;
		}
		cout << endl;
	}
	cout << endl;
	cout << "newSa" << endl;
	for(auto i = newSa.begin(); i != newSa.end(); ++i){
		for(auto j = i->begin(); j != i->end(); ++j){
			cout << *j;
		}
		cout << endl;

	}
	cout << endl;
	cout << "newE" << endl;
	for(auto i = newE.begin(); i != newE.end(); ++i){
		for(auto j = i->begin(); j != i->end(); ++j){
			cout << *j;
		}
		cout << endl;
	}
	cout << endl;

	cout << "new_look_up_s" << endl;
	for(auto i = new_look_up_s.begin(); i != new_look_up_s.end(); ++i){
		for(auto j = i->begin(); j != i->end(); ++j){
			cout << *j << " ";
		}
	}
	cout << endl;
	cout << "new_look_up_sa" << endl;
	for(auto i = new_look_up_sa.begin(); i != new_look_up_sa.end(); ++i){
		for(auto j = i->begin(); j != i->end(); ++j){
			cout << *j << " ";
		}
	}
	cout << endl;
}

/* Methods related to the "dfa common interface" */

vector<set<list<symbol_>>> AngluinDfa::group_phrases_into_states() const{
	vector<set<list<symbol_>>> groups(1, set<list<symbol_>>());

	auto it = S.begin();
	auto it_look_up = look_up_s.begin();
	auto it_sa = Sa.begin();
	auto it_look_up_sa = look_up_sa.begin();

	vector<int> current_state = *it_look_up;

	//Scorro lungo S
	while(it != S.end() && it_look_up != look_up_s.end()){
		//Se il prefisso NON corrisponde allo stato corrente viene creato un nuovo set di prefissi per il nuovo stato
		if(current_state != *it_look_up){
			current_state = *it_look_up;
			groups.emplace_back(set<list<symbol_>>());
		}
		//Il prefisso viene inserito nell'insieme di appartenenza
		groups.back().emplace(*it);

		//Cerco altri prefissi corrispondenti allo stato corrente in Sa
		while(it_look_up_sa != look_up_sa.end() && current_state == *it_look_up_sa && it_sa != Sa.end()){
			groups.back().emplace(*it_sa);

			++it_sa;
			++it_look_up_sa;
		}

		++it;
		++it_look_up;
	}

	//Alla fine devo avere scorso completamente sia la tabella S che quella Sa
	if(it != S.end() || it_look_up != look_up_s.end() || it_sa != Sa.end() || it_look_up_sa != look_up_sa.end()){
		cerr << "Error in AngluinDfa :: group_phrases_into_states" << endl;
		throw 0;
	}

	return groups;
}

vector<set<list<symbol_>>> AngluinDfa::sort_states(vector<set<list<symbol_>>> states, vector<vector<symbol_>>& sorted_phrases){
	vector<symbol_> sorted_alphabet = sort_alphabet();

	set<set<list<symbol_>>> visited_states;
	vector<set<list<symbol_>>> sorted_states;
	list<set<list<symbol_>>> reached_states;
	list<vector<symbol_>> reached_phrases;

	reached_states.push_back(states[get_start_index(states)]);
	reached_phrases.push_back(vector<symbol_>());

	//L'algoritmo va avanti fino a che non ho esplorato il dfa in ampiezza completamente,
	//ovvero fino a che il numero di stati ancora non raggiunti arriva a 0.
	int remaining_states = states.size();
	while(remaining_states > 0){
		//Prelevo lo stato da visitare e la stringa con la quale è stato raggiunto
		//tramite esplorazione in ampiezza
		set<list<symbol_>> state = reached_states.back();
		vector<symbol_> phrase = reached_phrases.back();
		reached_states.pop_back();
		reached_phrases.pop_back();

		//Se lo stato era stato raggiunto ma non visitato
		if(visited_states.find(state) == visited_states.end()){
			//riduco il numero di stati da raggiungere
			--remaining_states;

			//inserisco lo stato tra quelli visitati e tra quelli ordinati in ampiezza
			visited_states.insert(state);
			sorted_states.push_back(state);
			sorted_phrases.push_back(phrase);

			//per ogni simbolo a aggiungo l'a-sucessore dello stato agli stati raggiunti
			for(symbol_ s : sorted_alphabet){
				vector<symbol_> next_phrase = phrase;
				next_phrase.push_back(s);
				set<list<symbol_>> next_state = states[get_index_from_transition(state, s, states)];
				if(visited_states.find(next_state) == visited_states.end()){
					reached_states.push_front(next_state);
					reached_phrases.push_front(next_phrase);
				}
			}
		}
	}

	return sorted_states;
}

int AngluinDfa::get_start_index(vector<set<list<symbol_>>> states){
	int i = 0;
	//Cerco tra gli insiemi di prefissi quello che contiene la stringa vuota, che corrisponde allo stato iniziale
	//e ne restituisco l'indice
	for(auto s : states){
		if(s.find(list<symbol_>(1, "")) != s.end()){
			return i;
		}
		++i;
	}

	cerr << "Error in AngluinDfa::get_start_index" << endl;
	throw 0;
	return i;
}

int AngluinDfa::get_index_from_transition(set<list<symbol_>> current_state, symbol_ sym, vector<set<list<symbol_>>> all_states){
	int i = 0;

	//Cerco tra tutti gli stati uno che contenga una parola costruita a partire da una parola dello stato corrente
	//a cui si aggiunge come suffisso il simbolo sym
	for(auto state : all_states){
		for(auto word : current_state){
			word.push_back(sym);
			word.remove("");
			if(state.find(word) != state.end()){
				return i;
			}
		}
		++i;
	}

	//Se non viene trovato alcuno stato significa che il Dfa è aperto e non si dovrebbe richiamare questa funzione.
	cerr << "Error in AngluinDfa::get_state_from_transition" << endl;
	throw 0;
}

void AngluinDfa::update_state_table(){
	vector<vector<symbol_>> depth_sorted_phrases;

	vector<set<list<symbol_>>> states = group_phrases_into_states();
	states = sort_states(states, depth_sorted_phrases);

	vector<symbol_> sorted_alphabet = sort_alphabet();

	//Ho copiato i dati da lista a vettore perchè in generale per i passi del learner ho bisogno di aggiungere ed eliminare
	//elementi nel mezzo, ma in questo caso specifico è necessaria una lettura ad accesso randomico che è costante per il vettore,
	//ma linerare per la lista.
	vector<vector<int>> look_up_s_vec;
	vector<vector<int>> look_up_sa_vec;
	look_up_s_vec.insert(look_up_s_vec.end(), look_up_s.begin(), look_up_s.end());
	look_up_sa_vec.insert(look_up_sa_vec.end(), look_up_sa.begin(), look_up_sa.end());


	state_table_.clear();
	int i = 0;
	//Creo un DfaState per ogni diverso stato individuato indicando se è accettante e la frase di profondità.
	for(auto s : states){
		bool accepting;
		int index = get_index(S, *s.begin());
		if(index != -1){
			accepting = static_cast<bool>(look_up_s_vec[index][0]);
		}
		else{
			index = get_index(Sa, *s.begin());
			if(index != -1){
				accepting = static_cast<bool>(look_up_sa_vec[index][0]);
			}
			else{
				cerr << "Error in AngluinDfa::update_state_table" << endl;
				throw 0;
			}
		}

		state_table_.push_back(DfaState(accepting, depth_sorted_phrases[i], i));
		++i;
	}

	i = 0;
	//Per ogni DfaState e per ogni simbolo setto una transizione uscente e una entrante tra i 2 stati coinvolti
	for(auto s : states){
		for(symbol_ sym : sorted_alphabet){
			DFA_STATE_ j = get_index_from_transition(s, sym, states);
			state_table_[i].set_transition(sym, &state_table_[j]);
			state_table_[j].set_incoming_transiction(std::make_pair(&state_table_[i], sym));
		}
		++i;
	}

	num_states_ = state_table_.size();
}

vector<DfaState> :: iterator AngluinDfa::begin(){
	return state_table_.begin();
};

vector<DfaState> :: iterator AngluinDfa::end(){
	return state_table_.end();
};

void AngluinDfa::print_state_table(){
	cout << endl;
	for(auto state : *this){
		state.print();
	}
}
