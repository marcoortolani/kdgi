#include "angluindfa.h"
#include "concretedfa.h"
#include <iostream>
#include <math.h>

void AngluinDfa::clear_new_tables(){
	cout << "clear new tables" << endl;
	if(newS.size() > 0)
		newS.clear();
	if(newSa.size() > 0)
		newSa.clear();
	if(newE.size() > 0)
		newE.clear();
	if(new_look_up_s.size() > 0)
		new_look_up_s.clear();
	if(new_look_up_sa.size() > 0)
		new_look_up_sa.clear();
	cout << "done." << endl;
}

bool AngluinDfa::correct_order(vector <int> state1, vector <int> state2){
	if(state1.size() != state2.size()){
		cout << "Error in correct_order" << endl;
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
		cout << "Error in equal_states" << endl;
	}

	for(unsigned int i=0; i< state1.size(); i++){
		if(state1[i] != state2[i]){
			return i;
		}
	}

	return -1;
}

bool AngluinDfa::equal_vectors(vector <string> a, vector <string> b) const{

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

vector <string> AngluinDfa::concat_vectors(vector <string> a, vector <string> b) const{

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

int AngluinDfa::get_index(vector <vector<string>> S_Sa_E, vector <string> s) const {

    int size = S_Sa_E.size();

    for(int i=0; i<size; i++){
        if(equal_vectors(S_Sa_E[i],s)){
            return i;
        }
    }

    return -1;
}

int AngluinDfa::contain_phrase(vector<string> str){

	for(unsigned int i=0; i < S.size(); i++){
		for(unsigned int j=0; j < E.size(); j++){

			vector <string> str_concat = concat_vectors(S[i], E[j]);
			if(equal_vectors(str,str_concat)){
				return look_up_s[i][j];
			}
		}
	}

	for(unsigned int i=0; i < Sa.size(); i++){
			for(unsigned int j=0; j < E.size(); j++){

				vector <string> str_concat = concat_vectors(Sa[i], E[j]);
				if(equal_vectors(str,str_concat)){
					return look_up_sa[i][j];
				}
			}
		}

	return -1;
}

void AngluinDfa::sort_rows(string table, vector <int> sort){

	vector <vector<string>>* S_Sa;
	vector <vector<int>>* look_up_s_sa;

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
	vector <vector<int>> sorted_look_up_s_sa = vector<vector<int>>(dim_sort, vector<int>());
	for(int i=dim_sort-1; i>=0; i--){
		vector <int> row = (*look_up_s_sa)[sort[i]];
		look_up_s_sa->erase(look_up_s_sa->begin()+sort[i]);

		sorted_S_Sa.insert(sorted_S_Sa.begin(), (*S_Sa)[sort[i]]);
		S_Sa->erase(S_Sa->begin()+sort[i]);

		sorted_look_up_s_sa[i] = row;
	}

	for(int i=0; i<dim_sort; i++){
		look_up_s_sa->push_back(sorted_look_up_s_sa[i]);
		S_Sa->push_back(sorted_S_Sa[i]);
	}
}

void AngluinDfa::insert_row(string table, vector<string> sa, vector<int> queries){

	if(queries.size() != E.size()){
		cout << "Error in AngluinDfa: queries and E size dimension must agree" << endl;
	}

	vector <vector<string>>* S_Sa;
	vector <vector<int>>* look_up_s_sa;

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

	vector <int> state = vector <int>();

	for(unsigned int i=0; i<queries.size(); i++){
		if(queries[i]){
			state.push_back(1);
		}
		else{
			state.push_back(0);
		}
	}

	bool added_row = false;
	for(unsigned int i=0; i<S_Sa->size() && !added_row; i++){
		if(correct_order(state, (*look_up_s_sa)[i])){
			look_up_s_sa->insert(look_up_s_sa->begin()+i, state);
			S_Sa->insert(S_Sa->begin()+i, sa);
			added_row = true;
		}
	}

	if(!added_row){
		look_up_s_sa->push_back(state);
		S_Sa->push_back(sa);
	}
}

void AngluinDfa::delete_row(vector<string> sa){
	int index = get_index(Sa, sa);
	if(index != -1){
		Sa.erase(Sa.begin()+index);
		look_up_sa.erase(look_up_sa.begin()+index);
	}
	index = get_index(newSa, sa);
	if(index != -1){
		newSa.erase(newSa.begin()+index);
		new_look_up_sa.erase(new_look_up_sa.begin()+index);
	}
	return;
}

bool AngluinDfa::get_inconsistent_phrases(vector <vector<string>>& strings_to_verify){

    int size = look_up_s.size();
    if(size < 2){
        return true;
    }

    for(int i=0; i<size-1; i++){
        if(look_up_s[i] == look_up_s[i+1]){
            int alpha_size = alphabet_.size();
            for(int j=0; j<alpha_size; j++){

                int index1, index2;
                vector <int> state1, state2;
                vector <string> str1 = concat_vectors(S[i], vector<string>(1, alphabet_[j]));
                vector <string> str2 = concat_vectors(S[i+1], vector<string>(1, alphabet_[j]));

                index1 = get_index(S, str1);
                if(index1 != -1){
                    state1 = look_up_s[index1];
                }
                else{
                    index1 = get_index(Sa, str1);
                    state1 = look_up_sa[index1];
                }

                index2 = get_index(S, str2);
                if(index2 != -1){
                    state2 = look_up_s[index2];
                }
                else{
                    index2 = get_index(Sa, str2);
                    state2 = look_up_sa[index2];
                }

                if(index1 == -1 || index2 == -1){
                    cout << "Error in function get_inconsistent_string" << endl;
                    cout << "string1 :";
                    for(unsigned int j=0; j<str1.size(); j++){
                    	cout << str1[j];
                    }
                    cout << endl;
            		cout << "string2 :";
            		for(unsigned int j=0; j<str2.size(); j++){
						cout << str2[j];
					}
            		cout << endl;
                }

                int indexE = equal_states(state1,state2);
                if(indexE != -1){

                    vector <string> new_suffix = concat_vectors(vector<string>(1, alphabet_[j]), vector<string>(E[indexE]));

                    clear_new_tables();
                    newE = vector<vector<string>>(1, new_suffix);

                    for(unsigned int i1=0; i1<S.size(); i1++){
                    	vector <string> str = concat_vectors(S[i1], new_suffix);
                    	int contain = contain_phrase(str);
                    	new_look_up_s.push_back(vector<int>(1,contain));
                    	if(contain == -1){
                    		strings_to_verify.push_back(str);
                    	}
                    }

                    for(unsigned int i1=0; i1<Sa.size(); i1++){
						vector <string> str = concat_vectors(Sa[i1], new_suffix);
						int contain = contain_phrase(str);
						new_look_up_sa.push_back(vector<int>(1,contain));
						if(contain == -1){
							strings_to_verify.push_back(str);
						}
					}
                    for(unsigned int i1=0; i1<strings_to_verify.size(); i1++){
                    	cout << strings_to_verify[i1] << endl;
                    }
                    print_new_tables();
                    return false;
                }
            }
        }
    }
    return true;
}

bool AngluinDfa::get_open_phrases(vector<vector<string>>& strings_to_verify){

    int size_s = look_up_s.size();
    int size_sa = look_up_sa.size();

    for(int i=0; i<size_sa; i++){

        bool state_not_found = true;
        for(int j=0; j<size_s && state_not_found; j++){
            if(equal_states(look_up_sa[i], look_up_s[j]) == -1){
                state_not_found = false;
            }
        }

        if(state_not_found){

        	clear_new_tables();
        	strings_to_verify = vector <vector<string>>();
        	newS.push_back(Sa[i]);
        	new_look_up_s = vector <vector<int>>(1, vector<int>(E.size(), 0));

        	for(unsigned int j=0; j<E.size(); j++){
        		vector <string> str = concat_vectors(newS[0], E[j]);
        		int contained = contain_phrase(str);
        		new_look_up_s[0][j] = contained;
        		if(contained == -1){
        			strings_to_verify.push_back(str);
        		}
        	}

        	new_look_up_sa = vector <vector<int>>();
        	for(unsigned int j=0; j<alphabet_.size(); j++){
				vector<string> str = Sa[i];
				str.push_back(alphabet_[j]);
				newSa.push_back(str);
				new_look_up_sa.push_back(vector<int>(E.size(), 0));
				for(unsigned int k=0; k<E.size(); k++){
					str = concat_vectors(newSa[j], E[k]);
					int contained = contain_phrase(str);
					new_look_up_sa[j][k] = contained;
					if(contained == -1){
						strings_to_verify.push_back(str);
					}
				}
			}
            return false;
        }
    }
    return true;
}

void AngluinDfa::get_phrases_from_counterexample(vector<vector<string>>& strings_to_verify, vector<string> str){

	clear_new_tables();
	strings_to_verify = vector <vector<string>>();
	vector <vector<string>> strings_to_verify_sa = vector <vector<string>>();

	for(unsigned int i=0; i < str.size(); i++){
		vector<string> substr;
		substr.assign(str.begin(), str.begin()+i+1);

		if(get_index(S, substr)== -1){

			newS.push_back(substr);
			new_look_up_s.push_back(vector<int>(E.size(), 0));

			for(unsigned int j=0; j<E.size(); j++){
				vector <string> str1 = concat_vectors(substr, E[j]);
				int contain = contain_phrase(str1);
				new_look_up_s[new_look_up_s.size()-1][j] = contain;
				if(contain == -1){
					cout << "new_look_up_s "<< i << " " << E[j]<< ":" << str1 << endl;
					strings_to_verify.push_back(str1);
				}
			}

			for(unsigned int j=0; j<alphabet_.size(); j++){
				vector<string> stra = concat_vectors(substr, vector<string>(1, alphabet_[j]));
				newSa.push_back(stra);
				new_look_up_sa.push_back(vector<int>(E.size(), 0));
				for(unsigned int k=0; k<E.size(); k++){
					vector <string> str1 = concat_vectors(stra, E[k]);
					int contain = contain_phrase(str1);
					new_look_up_sa[new_look_up_sa.size()-1][k] = contain;
					if(contain == -1){
						cout << "new_look_up_sa "<< i << " " << alphabet_[j] << " " << E[k]<< ":" << str1 << endl;
						strings_to_verify_sa.push_back(str1);
					}
				}
			}
		}
	}

	strings_to_verify.insert(strings_to_verify.end(), strings_to_verify_sa.begin(), strings_to_verify_sa.end());
	cout << "strings_to_verify size: " << strings_to_verify.size();
	for(unsigned int j=0; j<strings_to_verify.size(); j++){
		cout << strings_to_verify[j] << endl;
	}
	print_new_tables();
}

void AngluinDfa::extend_column(vector <vector<string>>& strings_to_verify, vector<bool>* mem_query_res){

	unsigned int n = 0;
	vector <int> sort_S = vector<int>(0);

	for(unsigned int i=0; i<new_look_up_s.size(); i++){

		if(new_look_up_s[i][0] != -1){
			look_up_s[i].push_back(new_look_up_s[i][0]);
		}
		else{

			if(n < mem_query_res->size()){
				look_up_s[i].push_back((*mem_query_res)[n]);
				n++;
			}
			else{
				cout << "Error in extend_column" << endl;
			}
		}

		if(look_up_s[i][look_up_s[i].size()-1] == 1){
			sort_S.push_back(i);
		}
	}

	vector <int> sort_Sa = vector<int>(0);

	for(unsigned int i=0; i<new_look_up_sa.size(); i++){

		if(new_look_up_sa[i][0] != -1){
			look_up_sa[i].push_back(new_look_up_sa[i][0]);
		}
		else{

			if(n < mem_query_res->size()){
				look_up_sa[i].push_back((*mem_query_res)[n]);
				n++;
			}
			else{
				cout << "Error in extend_column" << endl;
			}
		}

		if(look_up_sa[i][look_up_sa[i].size()-1] == 1){
			sort_Sa.push_back(i);
		}
	}

	sort_rows("S", sort_S);
	sort_rows("Sa", sort_Sa);

	E.push_back(newE[0]);

	clear_new_tables();
}

void AngluinDfa::extend_rows(vector <vector<string>>& strings_to_verify, vector <bool>* mem_query_res){

	int s_rows = newS.size();
	int sa_rows = newSa.size();

    if(false){
    	cout << "Error in AnlguinDfa::extend_row: E and mem_query_res dimension must agree" << endl;
    	cout << "total_rows :" << endl;
    	cout << "s_rows :" << s_rows << endl;
    	cout << "sa_rows :" << sa_rows << endl;
    }

    unsigned int n = 0;

    for(unsigned int i=0; i<new_look_up_s.size(); i++){
    	for(unsigned int j=0; j<new_look_up_s[i].size(); j++){
    		if(new_look_up_s[i][j] == -1){
    			if(n < mem_query_res->size()){
    				new_look_up_s[i][j] = (*mem_query_res)[n];
    				n++;
    			}
    			else{
    				cout << "Error in extend_row" << endl;
    			}
    		}
    	}
    }

    for(unsigned int i=0; i<new_look_up_sa.size(); i++){
		for(unsigned int j=0; j<new_look_up_sa[i].size(); j++){
			if(new_look_up_sa[i][j] == -1){
				if(n < mem_query_res->size()){
					new_look_up_sa[i][j] = (*mem_query_res)[n];
					n++;
				}
				else{
					cout << "Error in extend_row, n:" << n << endl;
				}
			}
		}
	}

    for(unsigned int i=0; i<newSa.size(); i++){
		insert_row("Sa", newSa[i], new_look_up_sa[i]);
	}

    for(unsigned int i=0; i<newS.size(); i++){
    	delete_row(newS[i]);
		insert_row("S", newS[i], new_look_up_s[i]);
    }

    clear_new_tables();
}

ConcreteDfa* AngluinDfa::gen_concrete_dfa() const{

    if(look_up_s.size() == 0){
        return new ConcreteDfa(0, alphabet_);
    }

    vector <vector<int>> states;
    vector <int> indices;
    int start_state;

    for(unsigned int i=0; i<look_up_s.size(); i++){
        if(states.empty() || equal_states(look_up_s[i], states[states.size()-1]) != -1){
            states.push_back(look_up_s[i]);
            indices.push_back(i);
        }

        if(S[i].size() == 1 && S[i][0] == ""){
            start_state = states.size()-1;
            cout << "start_state: " << start_state << endl;
        }
    }

    ConcreteDfa* dfa_res = new ConcreteDfa(states.size(), alphabet_, start_state);

    for(unsigned int i=0; i<states.size(); i++){

    	for(unsigned int j=0; j<alphabet_.size(); j++){

            vector<string> str = concat_vectors(S[indices[i]], vector<string>(1,alphabet_[j]));

            int index = get_index(S, str);
            vector <int> next_val;

            if(index != -1){
                next_val = look_up_s[index];
            }
            else{
                index = get_index(Sa, str);

                if(index != -1){
                    next_val = look_up_sa[index];
                }
                else{
                    cout << "L' elemento str: " << str << " non è stato trovato." << endl;
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

	for(unsigned int i=0; i<E.size(); i++){
		cout << "\t\"";
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

		vector <int> state = look_up_s[i];
		for(unsigned int k=0; k<E.size(); k++){
			cout << "\t " << state[k];
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

		vector <int> state = look_up_sa[i];
		for(unsigned int k=0; k<E.size(); k++){
			cout << "\t " << state[k];
		}
		cout << endl;
	}
}

void AngluinDfa::print_new_tables(){

	cout << "newS";
	for(unsigned int i=0; i<newS.size(); i++){
		cout << newS[i];
	}
	cout << endl;
	cout << "newSa";
	for(unsigned int i=0; i<newSa.size(); i++){
		cout << newSa[i];
	}
	cout << endl;
	cout << "newE";
	for(unsigned int i=0; i<newE.size(); i++){
		cout << newE[i];
	}
	cout << endl;

	cout << "new_look_up_s" << endl;
	for(unsigned int i=0; i<new_look_up_s.size(); i++){
		cout << new_look_up_s[i] << endl;
	}
	cout << endl;
	cout << "new_look_up_sa" << endl;
	for(unsigned int i=0; i<new_look_up_sa.size(); i++){
		cout << new_look_up_sa[i] << endl;
	}
	cout << endl;
}

bool AngluinDfa::membership_query(vector<string> phrase) const {
	Dfa* concrete_dfa = gen_concrete_dfa();
	bool result = concrete_dfa->membership_query(phrase);
	free(concrete_dfa);
	return result;
}

AngluinDfa::AngluinDfa(vector <string> ab, vector <bool> mem_query_res) : Dfa(){

	if(mem_query_res.size() != ab.size()+1){
		cout << "Error in AngluinDfa Constructor" << endl;
		cout << "mem_query_res size must be equal to ab size + 1" << endl;
		return;
	}

	set_alphabet(ab);
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
	accepting_epsilon ? look_up_s = vector <vector<int>>(1, vector<int>(1,1)) : look_up_s = vector <vector<int>>(1, vector<int>(1,0));

	for(unsigned int i=0; i<ab.size(); i++){
		mem_query_res[i] ? look_up_sa.push_back(vector<int>(1,1)) : look_up_sa.push_back(vector<int>(1,0));
	}
}
