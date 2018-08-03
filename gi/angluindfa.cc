#include "angluindfa.h"

void AngluinDfa::clear_new_tables(){
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
}

bool AngluinDfa::correct_order(vector <int> state1, vector <int> state2){
	if(state1.size() != state2.size()){
		cerr << "Error in correct_order" << endl;
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

bool AngluinDfa::equal_lists(list <string> a, list <string> b) const{

	a.remove("");
	b.remove("");

	if(a.size() != b.size()){
		return false;
	}


	auto it_a = a.begin();
	auto it_b = b.begin();

	while(it_a != a.end()){
		if(*it_a != *it_b){
			return false;
		}
		++it_a;
		++it_b;
	}

	return true;
}

int AngluinDfa::get_index(list <list<string>> S_Sa_E, list <string> s) const {

    auto it = S_Sa_E.begin();
    int index = 0;
    while(it != S_Sa_E.end()){
    	if(equal_lists(*it, s)){
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

	while(it != S.end()){
		for(unsigned int j=0; j < E.size(); j++){
			list <string> str_concat = *it;
			str_concat.insert(str_concat.end(), E[j].begin(), E[j].end());
			str_concat.remove("");
			if(equal_lists(str,str_concat)){
				return (*it_look_up)[j];
			}
		}
		++it;
		++it_look_up;
	}

	it_look_up = look_up_sa.begin();
	it = Sa.begin();

	while(it != Sa.end()){
			for(unsigned int j=0; j < E.size(); j++){

				list <string> str_concat = (*it);
				str_concat.insert(str_concat.end(), E[j].begin(), E[j].end());
				str_concat.remove("");
				if(equal_lists(str,str_concat)){
					return (*it_look_up)[j];
				}
			}
			++it;
			++it_look_up;
		}

	return -1;
}

void AngluinDfa::insert_row(string table, list<string> sa, vector<int> queries){

	if(queries.size() != E.size()){
		cerr << "Error in NewAngluinDfa: queries and E size dimension must agree" << endl;
		throw 0;
	}

	list <list<string>>* S_Sa;
	list <vector<int>>* look_up_s_sa;

	if(table == "S"){
		S_Sa = &S;
		look_up_s_sa = &look_up_s;
	}
	else{
		S_Sa = &Sa;
		look_up_s_sa = &look_up_sa;
	}



	auto index = find(S_Sa->begin(), S_Sa->end(), sa);
	if(index != S_Sa->end()){return;}

	vector <int> state;

	for(unsigned int i=0; i<queries.size(); i++){
		if(queries[i]){
			state.push_back(1);
		}
		else{
			state.push_back(0);
		}
	}

	bool added_row = false;

	auto it = S_Sa->begin();
	auto it_look_up = look_up_s_sa->begin();

	while(it != S_Sa->end() && !added_row){
		if(correct_order(state, *it_look_up)){
			S_Sa->emplace(it, sa);
			look_up_s_sa->emplace(it_look_up, state);
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

	if(!added_row){
		look_up_s_sa->push_back(state);
		S_Sa->push_back(sa);
	}
}

void AngluinDfa::delete_row(list<string> sa){
	int index = get_index(Sa, sa);
	if(index != -1){
		auto it_look_up = look_up_sa.begin();
		advance(it_look_up, index);
		Sa.remove(sa);
		look_up_sa.erase(it_look_up);
	}
	index = get_index(newSa, sa);
	if(index != -1){
		auto it_look_up = new_look_up_sa.begin();
		advance(it_look_up, index);
		newSa.remove(sa);
		new_look_up_sa.erase(it_look_up);
	}
	return;
}

bool AngluinDfa::get_inconsistent_phrases(/*list <list<string>>&*/ map <list<string>, bool>* strings_to_verify){

    int size = look_up_s.size();
    if(size < 2){
        return true;
    }

    //used for performance reasons: random access is needed.
    vector <vector<int>> vec_look_up_s;
    vec_look_up_s.insert(vec_look_up_s.end(), look_up_s.begin(), look_up_s.end());
    vector <vector<int>> vec_look_up_sa;
	vec_look_up_sa.insert(vec_look_up_sa.end(), look_up_sa.begin(), look_up_sa.end());
	//less operations, more memory.

    auto it = S.begin();
    auto it1 = it;
    ++it1;
    auto it_look_up = look_up_s.begin();
    auto it_look_up1 = it_look_up;
    ++it_look_up1;
    while(it1 != S.end()){
        if(*it_look_up == *it_look_up1){
            int alpha_size = alphabet_.size();
            for(int j=0; j<alpha_size; j++){

                int index1, index2;
                vector <int> state1, state2;

                list <string> str1 = *it;
                str1.push_back(alphabet_[j]);
                str1.remove("");
                list <string> str2 = *it1;
				str2.push_back(alphabet_[j]);
				str2.remove("");

                index1 = get_index(S, str1);
                if(index1 != -1){
                    state1 = vec_look_up_s[index1];
                }
                else{
                    index1 = get_index(Sa, str1);
                    state1 = vec_look_up_sa[index1];
                }

                index2 = get_index(S, str2);
                if(index2 != -1){
                	state2 = vec_look_up_s[index2];
                }
                else{
                    index2 = get_index(Sa, str2);
                    state2 = vec_look_up_sa[index2];
                }

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

                int indexE = equal_states(state1,state2);
                if(indexE != -1){

                    list <string> new_suffix(1, alphabet_[j]);
                    new_suffix.insert(new_suffix.end(), E[indexE].begin(), E[indexE].end());
                    new_suffix.remove("");

                    clear_new_tables();
                    newE = vector<list<string>>(1, new_suffix);

                    for(auto i1 = S.begin(); i1 != S.end(); ++i1){
                    	list <string> str(*i1);
                    	str.insert(str.end(), new_suffix.begin(), new_suffix.end());
                    	str.remove("");
                    	int contain = contain_phrase(str);
                    	new_look_up_s.push_back(vector<int>(1,contain));
                    	if(contain == -1){
                    		(*strings_to_verify)[str];
                    	}
                    }

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
	for(auto i : look_up_sa){
			bool state_not_found = true;
        for(auto j : look_up_s){
            if(equal_states(i, j) == -1){
                state_not_found = false;
            }
        }

        if(state_not_found){
        	clear_new_tables();
        	newS.push_back(*it);
        	new_look_up_s = list <vector<int>>(1, vector<int>(E.size(), 0));

        	for(unsigned int j=0; j<E.size(); j++){
        		list <string> str(*(newS.begin()));
        		str.insert(str.end(), E[j].begin(), E[j].end());
        		str.remove("");
        		int contained = contain_phrase(str);
        		(*new_look_up_s.begin())[j] = contained;
        		if(contained == -1){
        			(*strings_to_verify)[str];
        		}
        	}

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
        	cerr << "Error in get_open_phrases" << endl;
			throw 0;
        }
    }
    return true;
}

void AngluinDfa::get_phrases_from_counterexample(map <list<string>, bool>* strings_to_verify, vector<string> str){
	clear_new_tables();

	for(unsigned int i=0; i < str.size(); i++){
		list<string> substr;
		substr.assign(str.begin(), str.begin()+i+1);

		if(get_index(S, substr)== -1){

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
	list<vector<int>> sorted_int;
	list<list<string>> sorted_string;
	auto it = S.begin();
	auto it_look_up = look_up_s.begin();
	auto it_new_look_up = new_look_up_s.begin();

	while(it_new_look_up != new_look_up_s.end()){
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

			S.erase(t);
			look_up_s.erase(t_look_up);
		}
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

	S.insert(S.end(), sorted_string.begin(), sorted_string.end());
	look_up_s.insert(look_up_s.end(), sorted_int.begin(), sorted_int.end());

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

	int s_rows = newS.size();
	int sa_rows = newSa.size();

    if(false){
    	cerr << "Error in AnlguinDfa::extend_row: E and mem_query_res dimension must agree" << endl;
    	cerr << "total_rows :" << endl;
    	cerr << "s_rows :" << s_rows << endl;
    	cerr << "sa_rows :" << sa_rows << endl;
    	throw 0;
    }

    auto it = newS.begin();
    for(auto it_look_up = new_look_up_s.begin(); it_look_up != new_look_up_s.end(); ++it_look_up){
    	auto itE = E.begin();
    	for(auto j = it_look_up->begin(); j != it_look_up->end(); ++j){
    		if(*j == -1){
    			list<string> str = *it;
    			str.insert(str.end(), itE->begin(), itE->end());
    			str.remove("");
    			auto pair = mem_query_res->find(str);
    			if(pair != mem_query_res->end()){
    				*j = pair->second;
    			}
    			else{
    				cerr << "Error in extend_row" << endl;
    				throw 0;
    			}
    		}
    		if(itE != E.end()){
    			++itE;
    		}
    		else{
    			cerr << "Error in extend_rows 0" << endl;
    		}
    	}
    	if(it != newS.end()){
    		++it;
    	}
    	else{
    		cerr << "Error in extend_rows 1" << endl;
    		throw 0;
    	}
    }

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
				cerr << "Error in extend_rows 0" << endl;
			}
		}
		if(it != newS.end()){
			++it;
		}
		else{
			cerr << "Error in extend_rows 1" << endl;
			throw 0;
		}
	}

    it = newSa.begin();
    auto it_look_up = new_look_up_sa.begin();
    while(it != newSa.end()){
		insert_row("Sa", *it, *it_look_up);

		++it;
		if(it_look_up != new_look_up_sa.end()){
			++it_look_up;
		}
		else{
			cerr << "Error in extend rows 3" << endl;
			throw 0;
		}
	}

    it = newS.begin();
    it_look_up = new_look_up_s.begin();
    while(it != newS.end()){
    	delete_row(*it);
		insert_row("S", *it, *it_look_up);

		++it;
		if(it_look_up != new_look_up_sa.end()){
			++it_look_up;
		}
		else{
			cerr << "Error in extend rows 4" << endl;
			throw 0;
		}
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
            //cout << "start_state: " << start_state << endl;
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

bool AngluinDfa::membership_query(vector<string> phrase) const {
	Dfa* concrete_dfa = gen_concrete_dfa();
	bool result = concrete_dfa->membership_query(phrase);
	free(concrete_dfa);
	return result;
}

AngluinDfa::AngluinDfa(vector <string> ab, map <list<string>, bool>* first_queries) : Dfa(){

	if(first_queries->size() != ab.size()+1){
		cerr << "Error in NewAngluinDfa Constructor" << endl;
		cerr << "mem_query_res size must be equal to ab size + 1" << endl;
		throw 0;
	}

	set_alphabet(ab);
	list<string> epsilon = list<string>(1,"");
	//list<string> epsilon;
	S.clear();
	S.push_back(epsilon);

	Sa.clear();
	for(unsigned int i=0; i<ab.size(); i++){
		Sa.push_back(list<string>(1,ab[i]));
	}

	E.clear();
	E.push_back(epsilon);

	bool accepting_epsilon = first_queries->find(epsilon)->second;
	accepting_epsilon ? look_up_s = list <vector<int>>(1, vector<int>(1,1)) : look_up_s = list <vector<int>>(1, vector<int>(1,0));

	for(unsigned int i=0; i<ab.size(); i++){
		bool accepting_ith_element = first_queries->find(list<string>(1, ab[i]))->second;
		accepting_ith_element ? look_up_sa.push_back(vector<int>(1,1)) : look_up_sa.push_back(vector<int>(1,0));
	}
}




/* Methods related to the "dfa common interface" */

vector<set<list<symbol_>>> AngluinDfa::group_phrases_into_states() const{
	vector<set<list<symbol_>>> groups(1, set<list<symbol_>>());

	auto it = S.begin();
	auto it_look_up = look_up_s.begin();
	auto it_sa = Sa.begin();
	auto it_look_up_sa = look_up_sa.begin();

	vector<int> current_state = *it_look_up;

	while(it != S.end() && it_look_up != look_up_s.end()){
		if(current_state != *it_look_up){
			current_state = *it_look_up;
			groups.emplace_back(set<list<symbol_>>());
		}
		groups.back().emplace(*it);

		while(it_look_up_sa != look_up_sa.end() && current_state == *it_look_up_sa && it_sa != Sa.end()){
			groups.back().emplace(*it_sa);

			++it_sa;
			++it_look_up_sa;
		}

		++it;
		++it_look_up;
	}

	if(it != S.end() || it_look_up != look_up_s.end() || it_sa != Sa.end() || it_look_up_sa != look_up_sa.end()){
		cerr << "Error in AngluinDfa::group_symbols" << endl;
		throw 0;
	}

	return groups;
}

vector<set<list<symbol_>>> AngluinDfa::sort_states(vector<set<list<symbol_>>> states, vector<vector<symbol_>>& sorted_phrases){

	vector<symbol_> sorted_alphabet = sort_alphabet();

	vector<symbol_> current_phrase;
	set<set<list<symbol_>>> visited_states;
	vector<set<list<symbol_>>> sorted_states;

	int remaining_states = states.size();

	do{
		set<list<symbol_>> state = states[get_start_index(states)];

		for(auto sym : current_phrase){
			state = states[get_index_from_transition(state, sym, states)];
		}

		list<symbol_> current_phrase_list;
		current_phrase_list.insert(current_phrase_list.end(), current_phrase.begin(), current_phrase.end());

		if(visited_states.find(state) == visited_states.end()){
			visited_states.insert(state);

			sorted_states.push_back(state);
			sorted_phrases.push_back(current_phrase);

			current_phrase.push_back(sorted_alphabet.front());
			--remaining_states;
		}
		else{
			//deve restituire la successiva stringa in ordine lessicografico
			//di dimensione pari o minore di quella passata per argomento.
			current_phrase = get_next_phrase(current_phrase);
		}
	}
	while(remaining_states > 0);

	return sorted_states;
}

int AngluinDfa::get_start_index(vector<set<list<symbol_>>> states){
	int i = 0;
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
	DFA_STATE_ i = 0;

	for(auto next_state : all_states){
		for(auto word : current_state){
			word.push_back(sym);
			word.remove("");
			if(next_state.find(word) != next_state.end()){
				return i;
			}
		}
		++i;
	}

	cerr << "Error in AngluinDfa::get_state_from_transiction" << endl;
	throw 0;
}

void AngluinDfa::update_state_table(){
	vector<vector<symbol_>> depth_sorted_phrases;

	vector<set<list<symbol_>>> states = group_phrases_into_states();
	states = sort_states(states, depth_sorted_phrases);

	vector<symbol_> sorted_alphabet = sort_alphabet();

	//Did this for random access
	vector<vector<int>> look_up_s_vec;
	vector<vector<int>> look_up_sa_vec;
	look_up_s_vec.insert(look_up_s_vec.end(), look_up_s.begin(), look_up_s.end());
	look_up_sa_vec.insert(look_up_sa_vec.end(), look_up_sa.begin(), look_up_sa.end());


	state_table_.clear();
	int i = 0;
	for(auto s : states){

		//Is the state accepting?
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
	for(auto s : states){
		for(symbol_ sym : sorted_alphabet){
			DFA_STATE_ j = get_index_from_transition(s, sym, states);
			//state_to_state_table_[i][j].push_back(sym);
			state_table_[i].set_transiction(sym, &state_table_[j]);
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
	//update_state_table();
	cout << endl;
	for(auto state : *this){
		state.print();
	}
}
