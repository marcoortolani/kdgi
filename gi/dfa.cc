/*
 * dfa.cc
 */


#include "dfa.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>	 	//Adopted for isstringstream
#include <map>
#include <list>
#include <cmath>
#include <algorithm>
#include <limits>
#include <random>

#include <boost/algorithm/string.hpp>

//#include "exceptions.h"
//#include "utilities.h"


using namespace std;

#define BUFFER_SIZE 1024

#define INF numeric_limits<int>::max()					/*!< Usually adopted for a non initialized state. */


// For Table Filling Algorithm
#define DFA_TF_STATE_N numeric_limits<SYMBOL>::max()		    // Unknown
#define DFA_TF_STATE_X numeric_limits<SYMBOL>::max() -1  	// Distinct state (accepting vs rejecting)

// For W-Method
#define LIMIT_OF_TESTSET_W_METHOD 1000000000
#define MAX_DIMENSION_WMETHOD_TEST_SET 500000

// Generation of random strings
#define ITERATION_LIMIT_FOR_EVERY_GENERAL_PROBLEM 6000000

template <typename T>
Dfa<T>::Dfa() {
	num_states_   = 0;
	start_state_  = 0;
	alphabet_	= vector<T>();
	ttable_ 	= vector<map<T,int>>();
	accepting_states_ = vector<int>();
}

template <typename T>
Dfa<T>::Dfa(const int n_state, const vector<T> alf, const int s_state){
	num_states_   = n_state;
	start_state_  = s_state;
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
	for(int i=0; i<n_state; ++i){
		for(T sym : alf)
			ttable_[i][sym]=0;
		accepting_states_.push_back(0);
	}
}

template <typename T>
Dfa<T>::Dfa(const int n_state, const vector<T> alf)
:Dfa(n_state, alf, 0){}

template <typename T>
Dfa<T>::Dfa(const int n_state, const vector<T> alf, const int s_state, vector<map<T,int>> tt_copy )
:Dfa(n_state, alf, 0){
	ttable_.reserve(tt_copy.size());
	copy(tt_copy.begin(),tt_copy.end(),back_inserter(ttable_));
}

template <typename T>
Dfa<T>::Dfa(const Dfa &d1)
:Dfa(d1.num_states_, d1.alphabet_, d1.start_state_, d1.ttable_){}

template <typename T>
const Dfa<T> &Dfa<T>::operator=(const Dfa<T> &d1)
{
	if(! (this == &d1) ) //avoid self-assignement
	{
            ttable_.clear();
	        num_states_ = d1.num_states_;

		    alphabet_.clear();
	        start_state_  = d1.start_state_;

			alphabet_.reserve(d1.alphabet_.size());
			copy(d1.alphabet_.begin(),d1.alphabet_.end(),back_inserter(alphabet_));

			ttable_.reserve(d1.ttable_.size());
			copy(d1.ttable_.begin(),d1.ttable_.end(),back_inserter(ttable_));

			accepting_states_.clear();
			accepting_states_.reserve(d1.accepting_states_.size());
			copy(d1.accepting_states_.begin(),d1.accepting_states_.end(),back_inserter(accepting_states_));

	}

	return *this;
}

template <typename T>
bool Dfa<T>::operator<(const Dfa<T> &d1) const
{
	return this->num_states_ < d1.num_states_;
}

template <typename T>
Dfa<T>::~Dfa(){
	alphabet_.clear();
	ttable_.clear();
}

template <typename T>
void Dfa<T>::set_num_state(int n){
	num_states_ = n;
}

template <typename T>
void Dfa<T>::set_alphabet(const vector<T> alf)
{
	alphabet_.clear();
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
}

template <typename T>
void Dfa<T>::set_ttable(const vector<map<T,int>> ext_ttable){
	ttable_.clear();
	ttable_.reserve(ext_ttable.size());
	copy(ext_ttable.begin(),ext_ttable.end(),back_inserter(ttable_));
}
/*
template <typename T>
Dfa<T> Dfa<T>::read_dfa_file(const string file_name)
{
	char nameDFA[BUFFER_SIZE];
	char line[BUFFER_SIZE];

	vector<T> alphabet_file;

	int counter = 0;
	int num_total_line = 0;
	int cstato = 0;
	char calfabeto[30];
	int ctransizione = 0;
	int current_line = 0;
	int start_state_ = 0;

	string n;

	ifstream read;
	string template_line;

	Dfa res;

	// Open connection to file
	read.open(file_name.c_str());

	if(read.is_open())
		cout << "File " << file_name << " is open."<< endl << endl;
	else{
		cerr << "Error opening: " << file_name << endl;
		throw readingExc();
	}


	// initial state
	start_state_ = 0;

	int dim_alphabet;
	// Read first line and set "num states", "dim alf" and "dfa name"
	read.getline(line,BUFFER_SIZE);
	counter = sscanf(line, "%d %d %s", &(dim_alphabet), &(res.num_states_), nameDFA);


	// Check if the first line is complete
	if(counter != 3){
		cout << "Error in first line of file" << endl;
		throw invalidFormat();
	}


	/////////////////////////  alphabet_  //////////////////////////////////
	// read the alphabet_
	alphabet_file.reserve(dim_alphabet);
	read.getline(line,BUFFER_SIZE);

	istringstream iss(line);

	counter=0;
	while (iss >> n){
		if(counter >= dim_alphabet)
			break;

		alphabet_file[counter++] = n;
	}

	// check read alphabet_
	if(counter != dim_alphabet){
		cout << "Error in second line of file: issue with size of alphabet_" << endl;

		if(alphabet_file)
			alphabet_file.clear();

		throw wrongAlphabetSize();
	}


	// Set alphabet_ for the current dfa
	res.set_alphabet(alphabet_file);

	if(alphabet_file)
		alphabet_file.clear();


	///////////// compute utility values//////////////////
	// expected lines of file
	num_total_line = (dim_alphabet+1)*(res.num_states_);

	// template of the all lines of the file
	template_line = (string)nameDFA+"[%d][%[^]]] = %d;";
	/////////////////////////////////////////////////////

	// allocate memory for ttable_
	res.ttable_.reserve(res.num_states_); = new int*[res.num_states_];

	// initialize ttable_ rows
	// "+2" is for algorithm like EDSM with states Type and Colour
	for(int i=0; i<res.num_states_; ++i){
		res.ttable_[i] = new int[res.dim_alphabet_+1];
		for(int j=0; j<res.dim_alphabet_+1; ++j)
			res.ttable_[i][j]=ND;
	}


	// Parsing the file
	while(!read.eof())
	{
		read.getline(line,BUFFER_SIZE);

		string cline = line;

		// Handler for last line
		string trimmedline = trim(cline);
		if(trimmedline == "")										// Happen only in the last line
			continue;

		++current_line;

		// Integrity check
		if(current_line > num_total_line){

			cerr << "ERROR - Line number greater than max" << endl;
			throw wrongLineNumber();
		}

		// Read line and set transition
		counter = sscanf(line, template_line.c_str(), &cstato, &calfabeto, &ctransizione);
		if(counter != 3)
			cerr << "ERROR in current line"<<current_line<<endl;

        //TODO: YOU could control that transition_symbol is in the alphabet_
		string transition_symbol = calfabeto;

		// It detects the row for type of state (accepting or rejecting)
		if(transition_symbol.compare(std::to_string(res.dim_alphabet_)) == 0)
			res.ttable_[cstato][std::stoi(transition_symbol)] = ctransizione;
		else
			res.ttable_[cstato][res.mapped_alphabet_[transition_symbol]] = ctransizione;
	}


	// Close connection
	read.close();

	return res;
}
*/


//https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file
template class Dfa<int>;
template class Dfa<char>;
template class Dfa<string>;