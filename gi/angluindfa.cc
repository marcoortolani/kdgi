/*
 * angluindfa.cpp
 *
 *  Created on: 17/mag/2018
 *      Author: marco
 */

#include "angluindfa.h"
#include <iostream>
#include <math.h>

bool AngluinDfa::equal_vectors(vector <string> a, vector <string> b){

	/*string stra = "";
	string strb = "";

	for(unsigned int i=0; i<a.size(); i++){
		stra += a[i];
	}

	for(unsigned int i=0; i<b.size(); i++){
			strb += b[i];
	}

	return stra == strb;*/

	a = concat_vectors(a, vector<string>(0));
	b = concat_vectors(b, vector<string>(0));

	if(a.size() != b.size()){
		return false;
	}

	for(unsigned int i=0; i<a.size(); i++){
		if(a[i] != b[i]){
			return false;
		}
	}

	return true;
}

vector <string> AngluinDfa::concat_vectors(vector <string> a, vector <string> b){

	if(a.size() == 0 && b.size() == 0){
		return a;
	}

	if(a.size() == 0 && b.size() != 0){
		a = b;
	}
	else{
		if(b.size() != 0){
			a.insert(a.end(), b.begin(), b.end());
		}
	}

	for(int i=a.size()-1; i>=0; i--){
		if(a[i] == ""){
			a.erase(a.begin() + i);
		}
	}

	return a;
}

bool AngluinDfa::get_inconsistent_string(vector <vector<string>>& prefixes_S, vector <vector<string>>& prefixes_Sa, vector<vector<string>>& suffixes){

    int size = look_up_s.size();
    if(size < 2){
        return true;
    }

    for(int i=0; i<size-1; i++){
        if(look_up_s[i] == look_up_s[i+1]){
            int alpha_size = alphabet.size();
            for(int j=0; j<alpha_size; j++){

                int index1, index2, val1, val2;
                vector <string> str1 = concat_vectors(S[i], vector<string>(1, alphabet[j]));
                vector <string> str2 = concat_vectors(S[i+1], vector<string>(1, alphabet[j]));

                index1 = get_index(S, str1);
                if(index1 != -1){
                    val1 = look_up_s[index1];
                }
                else{
                    index1 = get_index(Sa, str1);
                    val1 = look_up_sa[index1];
                }

                index2 = get_index(S, str2);
                if(index2 != -1){
                    val2 = look_up_s[index2];
                }
                else{
                    index2 = get_index(Sa, str2);
                    val2 = look_up_sa[index2];
                }

                if(index1 == -1 || index2 == -1){
                    cout << "Error in function get_inconsistent_string" << endl;
                    cout << "string1 :" << str1 << endl;
            		cout << "string2 :" << str2 << endl;

                }

                int val = val1 ^ val2;

                if(val != 0){

                    unsigned int indexE = 0;
                    while(val % 2 == 0){
                        indexE++;
                        val /= 2;
                    }

                    prefixes_S = S;
                    prefixes_Sa = Sa;
                    vector <string> saas1 = vector<string>(1, alphabet[j]);
                    vector <string> saas2 = vector<string>(E[indexE]);
                    vector <string> new_suffix = concat_vectors(saas1, saas2);
                    suffixes = vector<vector<string>>(1, new_suffix);

                    return false;
                }
            }
        }
    }

    return true;
}

bool AngluinDfa::get_open_string(vector<vector<string>>& prefixes_S, vector<vector<string>>& prefixes_Sa, vector<vector<string>>& suffixes){

    int size_s = look_up_s.size();
    int size_sa = look_up_sa.size();

    for(int i=0; i<size_sa; i++){

        int state = look_up_sa[i];
        bool state_not_found = true;

        for(int j=0; j<size_s && state_not_found; j++){
            if(state == look_up_s[j]){
                state_not_found = false;
            }
        }

        if(state_not_found){
        	prefixes_S.push_back(Sa[i]);
        	for(unsigned int j=0; j<alphabet.size(); j++){
        		vector<string> str = Sa[i];
        		str.push_back(alphabet[j]);
        		prefixes_Sa.push_back(str);
        	}
        	suffixes = E;
            return false;
        }
    }

    return true;
}

int AngluinDfa::get_index(vector <vector<string>> &S_Sa_E, vector <string> s){

    int size = S_Sa_E.size();

    for(int i=0; i<size; i++){
        if(equal_vectors(S_Sa_E[i],s)){
            return i;
        }
    }

    return -1;
}

void AngluinDfa::extend_column(vector <vector<string>>& prefixes_S, vector <vector<string>>& prefixes_Sa, vector <vector<string>>& suffixes, vector<vector<bool>>* mem_query_res){

	//TO-DO: sistemare questo controllo (mem_query_res deve avere dimensioni adeguate)
	if(suffixes.size()!=1){
		cout << "Error in AngluinDfa::extend_column(): suffixes must be of length 1" << endl;
	}

	if(get_index(E, suffixes[0]) != -1){
	        return;
	}

	int size = E.size();

	int val = pow(2,size);

	E.push_back(suffixes[0]);

	size = S.size();

	vector <int> sort_S = vector<int>(0);

	for(unsigned int i=0; i<prefixes_S.size(); i++){
		if((*mem_query_res)[i][0]){
			sort_S.push_back(i);
		}
	}

	vector <int> sort_Sa = vector<int>(0);

	for(unsigned int i=0; i<prefixes_Sa.size(); i++){
		unsigned int j = i + prefixes_S.size();
		if((*mem_query_res)[j][0]){
			sort_Sa.push_back(i);
		}
	}

	for(unsigned int i=0; i<sort_S.size(); i++){

		look_up_s[sort_S[i]] += val;
	}

	for(unsigned int i=0; i<sort_Sa.size(); i++){

		look_up_sa[sort_Sa[i]] += val;
	}

	sort_rows("S", sort_S);

	sort_rows("Sa", sort_Sa);
}

void AngluinDfa::sort_rows(string table, vector <int> sort){

	vector <vector<string>>* S_Sa;
	vector <int>* look_up_s_sa;

	if(table == "S"){
		S_Sa = &S;
		look_up_s_sa = &look_up_s;
	}
	else{
		S_Sa = &Sa;
		look_up_s_sa = &look_up_sa;
	}

	int dim = look_up_s_sa->size();
	int dim_sort = sort.size();

	if(dim_sort == 0 || dim_sort == dim){
		return;
	}

	vector <vector<string>> sorted_S_Sa;

	for(int i=dim_sort-1; i>=0; i--){

		int val = (*look_up_s_sa)[sort[i]];
		look_up_s_sa->erase(look_up_s_sa->begin()+sort[i]);

		sorted_S_Sa.insert(sorted_S_Sa.begin(), (*S_Sa)[sort[i]]);
		S_Sa->erase(S_Sa->begin()+sort[i]);

		sort[i] = val;
	}

	for(int i=0; i<dim_sort; i++){

		look_up_s_sa->push_back(sort[i]);
		S_Sa->push_back(sorted_S_Sa[i]);
	}
}

void AngluinDfa::extend_row(vector <vector<string>> s, vector<vector<string>> sa, vector <vector<bool>>* mem_query_res){

	//TO-DO: aggiustare questo controllo
	int total_rows = mem_query_res->size();
	int s_rows = s.size();
	int sa_rows = sa.size();

    if(total_rows == 0 || total_rows != s_rows + sa_rows ||(*mem_query_res)[0].size() != E.size()){
    	cout << "Error in AnlguinDfa::extend_row: E and mem_query_res dimension must agree" << endl;
    	cout << "total_rows :" << total_rows << endl;
    	cout << "s_rows :" << s_rows << endl;
    	cout << "sa_rows :" << sa_rows << endl;
    	if(total_rows > 0){
    		cout << "E: "<< E.size() << " Columns: " << (*mem_query_res)[0].size() << endl;
    	}
    	return;
    }

    for(unsigned int i=0; i<sa.size(); i++){
		insert_row("Sa", sa[i], (*mem_query_res)[i+s.size()]);
	}

    for(unsigned int i=0; i<s.size(); i++){

    	if(get_index(Sa, s[i]) != -1){
			delete_row(s[i]);
		}

		insert_row("S", s[i], (*mem_query_res)[i]);
    }
}

void AngluinDfa::insert_row(string table, vector<string> sa, vector<bool> queries){

	if(queries.size() != E.size()){
		cout << "Error in AngluinDfa: queries and E size dimension must agree" << endl;
	}

	vector <vector<string>>* S_Sa;
	vector <int>* look_up_s_sa;

	if(table == "S"){
		S_Sa = &S;
		look_up_s_sa = &look_up_s;
	}
	else{
		S_Sa = &Sa;
		look_up_s_sa = &look_up_sa;
	}

	int index = get_index(*S_Sa, sa);

	if(index != -1){
		return;
	}

	int value = 0;
	int p = 1;
	for(unsigned int i=0; i<queries.size(); i++){
		if(queries[i]){
			value += p;
		}
		p *= 2;
	}

	bool added_row = false;
	for(unsigned int i=0; i<S_Sa->size() && !added_row; i++){
		if(value <= (*look_up_s_sa)[i]){
			look_up_s_sa->insert(look_up_s_sa->begin()+i, value);
			S_Sa->insert(S_Sa->begin()+i, sa);
			added_row = true;
		}
	}

	if(!added_row){
		look_up_s_sa->push_back(value);
		S_Sa->push_back(sa);
	}
}

void AngluinDfa::delete_row(vector<string> sa){
	int index = get_index(Sa, sa);
	if(index != -1){
		Sa.erase(Sa.begin()+index);
		look_up_sa.erase(look_up_sa.begin()+index);
	}
	return;
}

AngluinDfa::AngluinDfa(vector <string> ab, vector <bool> mem_query_res){

	if(mem_query_res.size() != ab.size()+1){
		cout << "Error in AngluinDfa Constructor" << endl;
		cout << "mem_query_res size must be equal to ab size + 1" << endl;
		return;
	}

	alphabet = ab;
	string epsilon = "";
	S.clear();
	S.push_back(vector<string>(1,epsilon));

	Sa.clear();
	for(unsigned int i=0; i<ab.size(); i++){
		Sa.push_back(vector<string>(1,ab[i]));
	}

	E.clear();
	E.push_back(vector<string>(1,epsilon));

	bool accepting_epsilon = mem_query_res.back();
	accepting_epsilon ? look_up_s = vector <int>(1, 1) : look_up_s = vector <int>(1, 0);

	for(unsigned int i=0; i<ab.size(); i++){
		mem_query_res[i] ? look_up_sa.push_back(1) : look_up_sa.push_back(0);
	}
}

Dfa* AngluinDfa::gen_dfa(){

    if(look_up_s.size() == 0){
        return new Dfa(0, alphabet);
    }

    //states[n][0] contains the value of the n-th state, ie which strings will bring to an accepting state and which wont.
    //states[n][1] contains an index to access at one of the string that leads to that state.
    vector <int> states;
    vector <int> indices;
    int start_state;

    for(unsigned int i=0; i<look_up_s.size(); i++){
        if(states.empty() || look_up_s[i] != states[states.size()-1]){
            states.push_back(look_up_s[i]);
            indices.push_back(i);
        }

        if(S[i].size() == 1 && S[i][0] == ""){
            start_state = states.size()-1;
            cout << "start_state: " << start_state << endl;
        }
    }

    Dfa* dfa_res = new Dfa(states.size(), alphabet, start_state);

    for(unsigned int i=0; i<states.size(); i++){

    	cout << i << endl;

    	for(unsigned int j=0; j<alphabet.size(); j++){

            vector<string> str = concat_vectors(S[indices[i]], vector<string>(1,alphabet[j]));

            int index = get_index(S, str);
            int next_val;

            if(index != -1){
                next_val = look_up_s[get_index(S, str)];
            }
            else{
                index = get_index(Sa, str);

                if(index != -1){
                    next_val = look_up_sa[get_index(Sa, str)];
                }
                else{
                    cout << "L' elemento str: " << str << " non è stato trovato." << endl;
                }
            }

            bool found_next_state = false;
            for(unsigned int k=0; k<states.size() && !found_next_state; k++){
                if(next_val == states[k]){
                	cout << "\t" << alphabet[j] << "->" << k << endl;
                	dfa_res->set_ttable_entry(i, alphabet[j], k);
                    found_next_state = true;
                }
            }
        }

        if(states[i] % 2 == 1){
        	//this->set_accepting_state(i); // <-valido sempre
        	dfa_res->set_accepting_state(i); // <- i metodi devono essere pubblici
        	cout << "accepting" << endl;
        }
        else{
        	dfa_res->set_rejecting_state(i);
        	cout << "regecting" << endl;
        }
    }

    return dfa_res;
}

void AngluinDfa::print(){

	cout << "----------" << endl;
	cout << "S:" << S.size() << endl;
	cout << "----------" << endl;

	for(unsigned int i=0; i<E.size(); i++){
		cout << "\t";
		cout << "\"";
		for(unsigned int j=0; j<E[i].size(); j++){
			cout << E[i][j];
		}
		cout << "\"";
	}
	cout  << endl;

	for(unsigned int i=0; i<S.size(); i++){

		cout << "\"";
		for(unsigned j=0; j<S[i].size(); j++){
			cout << (S[i][j]);
		}
		cout  << "\"";

		int value = look_up_s[i];
		for(unsigned int k=0; k<E.size(); k++){
			cout << "\t " << value % 2;
			value /= 2;
		}
		cout << endl;
	}

	cout << "----------" << endl;
	cout << "Sa:" << Sa.size() << endl;
	cout << "----------" << endl;

	for(unsigned int i=0; i<E.size(); i++){
		cout << "\t";
		cout << "\"";
		for(unsigned int j=0; j<E[i].size(); j++){
			cout << E[i][j];
		}
		cout << "\"";
	}
	cout << endl;

	for(unsigned int i=0; i<Sa.size(); i++){

		cout << "\"";
		for(unsigned j=0; j<Sa[i].size(); j++){
			cout << (Sa[i][j]);
		}
		cout  << "\"";

		int value = look_up_sa[i];
		for(unsigned int k=0; k<E.size(); k++){
			cout << "\t " << value % 2;
			value /= 2;
		}
		cout << endl;
	}
}

void AngluinDfa::get_test(vector<vector<string>>& prefixes_S, vector<vector<string>>& prefixes_Sa, vector<vector<string>>& suffixes, vector<string> str){

	for(unsigned int i=0; i < str.size(); i++){
		vector<string> substr;
		substr.assign(str.begin(), str.begin()+i+1);

		if(get_index(S, substr)==-1){

			prefixes_S.push_back(substr);

			for(unsigned int j=0; j<alphabet.size(); j++){
				vector<string> stra = concat_vectors(substr, vector<string>(1, alphabet[j]));
				prefixes_Sa.push_back(stra);
			}
		}
	}
	suffixes = E;
}
