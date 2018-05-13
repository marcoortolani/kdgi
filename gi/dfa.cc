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

#include "exceptions.h"
#include "utilities.h"


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


Dfa::Dfa() {
	num_states_   = 0;
	start_state_  = 0;
	alphabet_	= vector<string>();
	ttable_ 	= vector<map<string,int>>();
	accepting_states_ = vector<int>();
}


Dfa::Dfa(const int n_state, const vector<string> alf, const int s_state){
	num_states_   = n_state;
	start_state_  = s_state;
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
	for(int i=0; i<n_state; ++i){
		for(string sym : alf)
			ttable_[i][sym]=0;
		accepting_states_.push_back(0);
	}
}


Dfa::Dfa(const int n_state, const vector<string> alf)
:Dfa(n_state, alf, 0){}


Dfa::Dfa(const int n_state, const vector<string> alf, const int s_state, vector<map<string,int>> tt_copy )
:Dfa(n_state, alf, 0){
	ttable_.reserve(tt_copy.size());
	copy(tt_copy.begin(),tt_copy.end(),back_inserter(ttable_));
}


Dfa::Dfa(const Dfa &d1)
:Dfa(d1.num_states_, d1.alphabet_, d1.start_state_, d1.ttable_){}


const Dfa &Dfa::operator=(const Dfa &d1)
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


bool Dfa::operator<(const Dfa &d1) const
{
	return this->num_states_ < d1.num_states_;
}


Dfa::~Dfa(){
	alphabet_.clear();
	ttable_.clear();
}


void Dfa::set_num_state(int n){
	num_states_ = n;
}


void Dfa::set_alphabet(const vector<string> alf)
{
	alphabet_.clear();
	alphabet_.reserve(alf.size());
	copy(alf.begin(),alf.end(),back_inserter(alphabet_));
}


void Dfa::set_ttable(const vector<map<string,int>> ext_ttable){
	ttable_.clear();
	ttable_.reserve(ext_ttable.size());
	copy(ext_ttable.begin(),ext_ttable.end(),back_inserter(ttable_));
}


int Dfa::get_dim_alphabet() const{
	return alphabet_.size();
}


Dfa Dfa::read_dfa_file(const string file_name)
{
	char nameDFA[BUFFER_SIZE];
	char line[BUFFER_SIZE];

	vector<string> alphabet_file;

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


	// read the alphabet_
	alphabet_file.reserve(dim_alphabet);
	read.getline(line,BUFFER_SIZE);

	istringstream iss(line);

	counter=0;
	while (iss >> n){
		if(counter >= dim_alphabet)
			break;

		alphabet_file.push_back(n);
	}

	// check read alphabet_
	if(counter != dim_alphabet){
		cout << "Error in second line of file: issue with size of alphabet_" << endl;

		//if(alphabet_file)
		//	alphabet_file.clear();

		throw wrongAlphabetSize();
	}


	// Set alphabet_ for the current dfa
	res.set_alphabet(alphabet_file);

	//if(alphabet_file)
	//	alphabet_file.clear();


	///////////// compute utility values//////////////////
	// expected lines of file
	num_total_line = (dim_alphabet+1)*(res.num_states_);

	// template of the all lines of the file
	template_line = (string)nameDFA+"[%d][%[^]]] = %d;";
	/////////////////////////////////////////////////////

	// allocate memory for ttable_
	res.ttable_.reserve(res.num_states_*alphabet_file.size());;

	// "+2" is for algorithm like EDSM with states Type and Colour
	res.accepting_states_.reserve(res.num_states_);
	for(int i=0; i<res.num_states_; ++i){
		for(string sym : alphabet_file)
			res.ttable_[i][sym]=ND;
		res.accepting_states_.push_back(0);	
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

		res.ttable_[cstato][transition_symbol] = ctransizione;
		// It detects the row for type of state (accepting or rejecting)
		if(transition_symbol.compare(std::to_string(res.get_dim_alphabet())) == 1)
			res.accepting_states_[cstato]=1;

	}

	// Close connection
	read.close();

	return res;
}

const vector<string> Dfa::get_alphabet() const{
	return alphabet_;
}

const vector<int> Dfa::get_accepting_states() const{
	return accepting_states_;
}

bool Dfa::is_accepting(int curr_state) const{
	return accepting_states_[curr_state];
}

int Dfa::get_num_states() const{
	return num_states_;
}

int Dfa::get_start_state(){
	return start_state_;
}

vector<map<string,int>> Dfa::get_ttable() const {
	return ttable_;
}

int Dfa::get_ttable(int i, string j){
	vector<string> alph=get_alphabet();
	if(i<num_states_ && std::find(alph.begin(), alph.end(), j) != alph.end())
		return ttable_[i][j];
	else{
		cerr<<"dfa::get_ttable: out of bound"<<endl;
		throw indexOutOfBoundTtable();
	}
}

void Dfa::set_ttable_entry(int i, string j, int v){
	vector<string> alph=get_alphabet();
	if(i<num_states_ && std::find(alph.begin(), alph.end(), j) != alph.end())
		ttable_[i][j]=v;
	else{
			cerr<<"dfa::set_ttable_: out of bound"<<endl;
			throw indexOutOfBoundTtable();
		}
}

void Dfa::set_accepting_state(int state_to_mark){
	if(state_to_mark<get_dim_alphabet())
		accepting_states_[state_to_mark]=1;
}

void Dfa::set_rejecting_state(int state_to_mark){
	if(state_to_mark<get_dim_alphabet())
		accepting_states_[state_to_mark]=0;
}

void Dfa::set_ttable_from_sequence(const vector<int> &sequence)
{
	vector<string> alph = get_alphabet();
	int n_col = alph.size()+1;
	if((sequence.size() % n_col) != 0 || (sequence.size() / n_col) != num_states_)
	{
		cerr << "Impossible set the ttable_.\nDimension of sequence isn't a multiple of dimension of alphabet_ +1\n or the sequence doesn't correspond to number of states " << endl;
		throw invalidParameters();
	}

	for(int i=0; i<num_states_; ++i){
		int cnt=0;
		for(string sym : alph){
			ttable_[i][sym] = sequence[(i*n_col)+cnt];
			++cnt;
		}
		if(sequence[i*(alph.size()+1)+alph.size()])
			accepting_states_[i]=1;
	}	
}

Dfa* Dfa::unionDFA(Dfa* dfa_hp)
{
	int count_state = dfa_hp->num_states_ + num_states_;


	// Instance of union dfa
	Dfa* union_dfa = new Dfa(count_state, alphabet_);
	for(int i=0; i<num_states_;++i)
		if(is_accepting(i))
			union_dfa->set_accepting_state(i);
	for(int j=0; j<dfa_hp->num_states_;++j)
		if(dfa_hp->is_accepting(j))
			union_dfa->set_accepting_state(j+num_states_);

	// Configuration of Union DFA
	// Smaller indexes are given to target dfa, while others to hypothesis
	for(int j=0; j<num_states_; ++j)									// Target automaton
		for(string sym : alphabet_)
			union_dfa->ttable_[j][sym] = ttable_[j][sym];


	for(int j=0; j<dfa_hp->num_states_; ++j)						// Hypothesis automaton
		for(string sym_hp : alphabet_)					// In union dfa, start state of HP dfa is recorded in "num_state" index of target dfa
			union_dfa->ttable_[num_states_+j][sym_hp] = dfa_hp->ttable_[j][sym_hp] + num_states_;

	return union_dfa;
}

string Dfa::random_ttable()
{
	string random_sequence;
	int extracted_number;
	random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, get_num_states()-1);
    uniform_int_distribution<> dis2(0, 1); //for accepting/rejecting column

	//cout<<"The extracted random ttable_ is :"<<endl;
	for(int row=0,num_element=0; row<get_num_states(); ++row)
	{
		for(string sym : get_alphabet())
		{
			extracted_number = dis(gen);

			ttable_[row][sym] = extracted_number;
			random_sequence += std::to_string(extracted_number);

			//cout<<ttable_[row][col]<<" ";
		}
		//cout<<endl;
		extracted_number = dis2(gen);
		if(extracted_number)
			set_accepting_state(row);
    }
    //cout<<endl<<random_sequence<<endl;

    return random_sequence;
}


