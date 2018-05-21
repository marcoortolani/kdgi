/*
 * angluindfa.h
 *
 *  Created on: 17/mag/2018
 *      Author: marco
 */

#ifndef ANGLUINDFA_H_
#define ANGLUINDFA_H_

#include <vector>
#include <string>
#include "dfa.h"

using namespace std;

class AngluinDfa : public Dfa{
private:
	vector <string> alphabet;
	vector <vector<string>> S;
	vector <vector<string>> Sa;
	vector <vector<string>> E;
	vector <int> look_up_s;
	vector <int> look_up_sa;

	bool equal_vectors(vector <string> a, vector <string> b);

	vector <string> concat_vectors(vector <string> a, vector <string> b);

	//Returns the index of the element of S or Sa equal to string s, -1 if none is found.
	int get_index(vector <vector<string>> &S_Sa_E, vector <string> s);

	//Orders the rows of S and look_up_s or Sa and look_up_sa following a state-value based order.
	void sort_rows(string table, vector <int> sort);

	void insert_row(string table, vector<string> sa, vector<bool> queries);

	void delete_row(vector<string> sa);

public:

	//Returns the first string that has to be added to S in order to make the look-up table consistent.
	bool get_inconsistent_string(vector <vector<string> >& prefixes_S, vector <vector<string> >& prefixes_Sa, vector<vector<string>>& suffixes);

	//Returns the first string that has to be added to E in order to make the look-up table closed.
	bool get_open_string(vector<vector<string>>& prefixes_S, vector<vector<string>>& prefixes_Sa, vector<vector<string>>& suffixes);

	void get_test(vector<vector<string>>& prefixes_S, vector<vector<string>>& prefixes_Sa, vector<vector<string>>& suffixes, vector<string> str);

	//Adds the string aE to E and update accordingly the look-up table
	void extend_column(vector<vector<string>>& prefixes_S, vector<vector<string>>& prefixes_Sa, vector<vector<string>>& suffixes, vector<vector<bool>>* mem_query_res);

	//Adds the string sa and all its prefixes if recursive is true to S and update accordingly the look-up table
	void extend_row(vector <vector<string>> s, vector <vector<string>> sa, vector <vector<bool>>* mem_query_res);

	AngluinDfa(vector <string> ab, vector <bool> mem_query_res);

	//Creates a dfa from the current look-up table (ie S, Sa, look_up_s, look_up_sa). It does not control if the look-up table is closed and consistent.
	Dfa* gen_dfa();

	void print();

};

#endif /* ANGLUINDFA_H_ */
