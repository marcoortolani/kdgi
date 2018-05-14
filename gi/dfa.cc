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

int Dfa::get_start_state() const{
	return start_state_;
}

vector<map<string,int>> Dfa::get_ttable() const {
	return ttable_;
}

int Dfa::get_ttable(int i, string j) const {
	vector<string> alph=get_alphabet();
	if(i<num_states_ && std::find(alph.begin(), alph.end(), j) != alph.end())
		return ttable_[i].at(j);
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
			union_dfa->set_ttable_entry(j,sym,get_ttable(j,sym));


	for(int j=0; j<dfa_hp->num_states_; ++j)						// Hypothesis automaton
		for(string sym_hp : alphabet_)					// In union dfa, start state of HP dfa is recorded in "num_state" index of target dfa
			union_dfa->set_ttable_entry(num_states_+j,sym_hp, (dfa_hp->get_ttable(j,sym_hp) + num_states_));

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
	for(int row=0; row<get_num_states(); ++row)
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

Dfa* Dfa::minimize_TF() const
{
	const int num_row = num_states_;


	// Matrix of distincts: table for record distinct states
	bool** distinct = new bool*[num_states_-1];								// Table of distinct pairs of states
	for(int i=0; i<num_row-1; ++i)											// E.g: 6 states generate a matrix with 5 rows and 6 columns (only 5 of 6 are actually used)
		distinct[i] = new bool[num_states_];


	// Initialization of distinct table
	for(int i=0; i<num_states_-1; ++i)
		for(int j=0; j<num_states_; ++j)
			distinct[i][j]=false;


	// Acceptor and rejector states are different by definition, so they are marked. (E.g.: 0 <= i <= 4, j=i+1 (1 <= j <= 5 during the first iteration).
	for(int i=0; i<(num_states_-1); ++i)
		for(int j=i+1; j<num_states_; ++j)
			if(is_accepting(i) != is_accepting(j))			// Check if one is an acceptor and the otherone no.
				distinct[i][j] = true;


	// Minimizing loop
	bool modified = true;
	while(modified)
	{
		modified = false;

		for(int i=0; i<(num_states_-1); ++i){
			for(int j=i+1; j<num_states_; ++j){
				if(!distinct[i][j]){

					for(string sym : get_alphabet())
					{
						int arrive_state_1 = get_ttable(i,sym);
						int arrive_state_2 = get_ttable(j,sym);


						if(arrive_state_1 == arrive_state_2)
							continue;


						// By definition, in the table of pair states turns out that j>i always,
						// then pairs shuould have i<j
						if(arrive_state_2 < arrive_state_1){
							int tmp = arrive_state_1;
							arrive_state_1 = arrive_state_2;
							arrive_state_2 = tmp;
						}


						// If arrive pair of states is distinct, then is start state pair as well (!)
						if(distinct[arrive_state_1][arrive_state_2]){
							distinct[i][j] = true;
							modified = true;
						}
					}
				}
			}
		}
	}


	//// In the following: operations need for a new instance of minimized automaton

	// A list of equivalent pairs of states is built providing a direct access.
	// If a state is marked with "ND", it have no equivalent state
	int equivalent_state[num_states_];
	for(int i=0; i<num_states_; ++i)
		equivalent_state[i] = ND;


	// It counts the number of state and assigns the eventually equivalent state
	// NB: It have to be checked that, for example, when I met the pairs (0,4) and (0,5), the pair (4,5) should not be count as another state
	int final_states_counter = num_states_;
	for(int i=0; i<(num_states_-1); ++i)
		for(int j=i+1; j<num_states_; ++j)
			if(!distinct[i][j] && equivalent_state[i] == ND && equivalent_state[j] == ND){
				equivalent_state[j] = i;
				final_states_counter--;
			}


	// Print information about equivalent states
	/*
	cout << "N of final states: " << final_states_counter << endl;
	cout << "Equivalent states: " << endl;
	for(int i=0; i<num_state; ++i)
		if(distinct[i] != ND)
			cout << "S:"<<i<<" --> "<<distinct[i]<<endl;
		else
			cout << "S:"<<i<<endl
	*/


	///////////////////////////////////////////////////////
	// A new instance of DFA is built for the minimized DFA
	Dfa* dfa_min = new Dfa(final_states_counter, alphabet_, 0);

	vector<map<string,int>> ttable__min = dfa_min->get_ttable();

	int count = 0;
	for(int i=0; i<num_states_; ++i){
		if(equivalent_state[i] == ND){
			for(string sym : get_alphabet())
				ttable__min[count][sym]=get_ttable(i,sym);
			count++;
		}
	}


	// Old transitions towards deleteted states are updated towards the new equivalent state
	for(int i=0; i<num_states_; ++i){
		if(equivalent_state[i] != ND)
		{
			for(int k=0; k<final_states_counter; ++k)
				for(string sym : get_alphabet())
					// Transition toward "i" state is substitutes with one towards equivalent state "equivalent_state[i]"
					if(ttable__min[k][sym] == i)
						ttable__min[k][sym] = equivalent_state[i];
		}
	}


	// Final update of state labels. E.g.: if 2 states before the state 6 have been collapsed,
	// the state 6 is now in the row 4 but old transitions have remained towards row 6
	// and they need to be updated.
	int equivalences_found_so_far= 0;
	for(int i=0; i<num_states_; ++i)
	{
		if(equivalent_state[i] != ND)
			equivalences_found_so_far++;

		if(equivalences_found_so_far != 0)
		{
			int nuova_label = i-equivalences_found_so_far;
			for(int k=0; k<final_states_counter; ++k)
				for(string sym : get_alphabet())
					if(ttable__min[k][sym] == i)
						ttable__min[k][sym] = nuova_label;
		}
	}


	// Free allocated memory
	if(distinct != NULL){
		for(int i=0; i<num_row -1; ++i){
			if(distinct[i] != NULL){
				delete[] distinct[i];
			}
		}
		delete [] distinct;
	}


	return dfa_min;
}

void Dfa::print_dfa_ttable(string title) const
{
	// It uses Mapped alphabet_


	// Print an header
	cout << endl<< "--------------------------" << endl;
	cout << title << endl;
	string header = "  ";
	for(int i=0; i<get_dim_alphabet(); ++i)
		header = header + " | "+ alphabet_[i];
	header = header + " - A";
	cout << header << endl;


	// Print transitions
	for(int i=0; i<num_states_; ++i){
		cout << "S"<<i<<"  ";

		for(string sym : get_alphabet())
		{
			// Transition values: a value or "ND"
			if(get_ttable(i,sym) == ND)
				cout << " N ";
			else
				cout << " "<< get_ttable(i,sym) <<"  ";
		}
		// Type of state
		if(is_accepting(i))
			cout << " Ac ";
		else
			cout << " Ri ";

		cout << endl;
	}

	cout << "--------------------------" << endl;
}

void Dfa::print_dfa_dot(string title, const char *file_path)
{
	ofstream myfile;
	myfile.open(file_path);


	// Initialization of strings with DOT code
	string header = "digraph "+title+" {\n";
	string start_state_ = "__start0 [label=\"\" shape=\"none\"];\n\n";

	start_state_ = start_state_ + "rankdir=LR;\nsize=\"8,5\";\n\n";


	//States
	string states = "";
	string shape = "";
	string style="";
	string color="";
	for(int i=0; i<num_states_; ++i)
	{
		//if(ttable_[i][dim_alphabet_] == DFA_STATE_UNREACHABLE)
		//	continue;

		if(is_accepting(i)){
			shape = "doublecircle";
			style = "rounded,filled";
		}
		else{
			shape = "circle";
			style = "filled";
		} 
		color="white";

		states = states + "s"+std::to_string(i)+" [style=\""+style+"\", color=\"black\", fillcolor=\""+color+"\" shape=\""+shape+"\", label=\""+std::to_string(i)+"\"];\n";
	}


	// Transitions
	string transitions = "";
	for(int i=0; i<num_states_; ++i){
		for(string sym : get_alphabet()){
			int arrive_state = get_ttable(i,sym);
			if(arrive_state == ND)
				continue;

			transitions = transitions + "s"+std::to_string(i)+" -> s"+std::to_string(arrive_state)+" [label=\""+sym+"\"];\n";
		}
	}


	string end = "__start0 -> 0;";
	string footer ="\n}";


	// Finally, it prints overall DOT code
	myfile << header << start_state_ << states << transitions << footer;

	myfile.close();
}

void Dfa::print_dfa_in_text_file(const string file_path)
{

	// Check if some transitions is "Undefined"
	for(int i=0; i<num_states_; ++i)
		for(string sym : get_alphabet())
			if(get_ttable(i,sym) == ND){
				cout << "ERROR: DFA is not completely defined, there ND transition" << endl;
				throw incompleteDfa();
			}


	// **********************
	//   WRITE DFA in FILE
	// **********************

	// Opena file
	ofstream myfile;
	myfile.open(file_path.c_str());


	// Write alphabet_ size
	myfile << std::to_string(get_dim_alphabet()) << " ";

	// Write num of states
	myfile << std::to_string(num_states_) << " ";

	// Write dfa name
	myfile << "dfa" << "\n";

	// Write alphabet_ symbols
	for(string sym : get_alphabet())
		myfile << sym << " ";
	myfile << "\n";


	// Write transition table
	for(int i=0; i<num_states_; ++i){
		for(string sym : get_alphabet())
		{
			myfile << "dfa[" <<std::to_string(i)<<"][";
			
			myfile << sym << "]="<< get_ttable(i,sym) <<";\n";
		}

		myfile << std::to_string(get_dim_alphabet()) << "]=";
		if(is_accepting(i))
			myfile << "1";
		else 
			myfile << "0";

		myfile <<";\n";
	}

	myfile.close();
}

int Dfa::get_arrive_state(vector<string> &dfa_string) const
{
	int state = get_start_state();
	int next_state=ND;

	for(string sym : dfa_string){
		next_state = get_ttable(state,sym);
		if(next_state == ND){
			state = ND;
			break;
		}
		state = next_state;
	}
	// It returns "ND" if the provided string is not compatible, otherwise it returns reached state
	return state;
}

bool Dfa::membership_query(vector<string> str)const{

	// Check if arrive_state is ND (for DFA without sink state)
	int arrive_state = get_arrive_state(str);
	if(arrive_state == ND)
		return false;

	if(is_accepting(arrive_state))
		return true;
	else
		return false;
}

/*
bool Dfa::compare_dfa( Dfa *dfa_to_compare , string method , ir_statistical_measures &stats1 , ir_statistical_measures &stats2 )
{
	boost::algorithm::to_lower(method);
	if(method != "w-method" && method != "random-walk")
	{
	   cerr<<"Pointed out method to use for comparison is wrong"<<endl;
	   throw invalidParameters();
	}

	vector<string> test_set;
	vector<string> test_set2;
	int n_states_dfa_to_compare = dfa_to_compare->get_num_states();
	int n_states_this_dfa = this->get_num_states();
	int num_pos_samples = 750;
	int num_neg_samples = 750;
	bool useRandomWalk=false;

        //If an exception happen (caused from called method here) automatically is rethrow to called function (furthermore there isn't dynamics memory to handle thus try catch isn't necessary)
		if (method == "w-method")
		{
		   //W-Method create very very many samples. To avoid this situation use W-Method alone if the state different isn't greater than 10
		   if( abs(n_states_this_dfa - n_states_dfa_to_compare) <= 10 )//Use W-Method
		   {
			   if( n_states_dfa_to_compare >= n_states_this_dfa )
			      test_set = this->get_w_method_test_set(dfa_to_compare,true);
			   else
			      test_set = dfa_to_compare->get_w_method_test_set(this,true);

			   if(test_set.size() < 100 || test_set.size()>MAX_DIMENSION_WMETHOD_TEST_SET) //too poor test set, use random walk
			      useRandomWalk=true;

		   }
		   else //use random walk
		      useRandomWalk=true;
	    }

	    if(method == "random-walk" || useRandomWalk) //with random walk samples are generated on target and on model too
	    {
				test_set={}; //to delete sample finally generated with W-Method
				#ifdef DEBUG_OBPA_PRINCIPAL
				   cout<<"We are using random-walk to generate samples"<<endl;
				#endif


				useRandomWalk = true; //for a correct return value in all cases
				//vector<string> sentence_temp;

				//convert the structure returned from random walk in a set<vector<SYMBOL>>
				for(auto &samples_set : this->generate_pos_neg_samples_without_weights(num_pos_samples,num_neg_samples) )
				{
					//sentence_temp = {};
					for(auto & sample : samples_set.first) //samples_set.first is a vector<string>
						test_set.push_back(sample);

					//test_set.push_back(sentence_temp);
				}

				for(auto &samples_set : dfa_to_compare->generate_pos_neg_samples_without_weights(num_pos_samples,num_neg_samples) )
				{
					//sentence_temp = {};
					for(auto & sample : samples_set.first) //samples_set.first is a vector<string>
						test_set2.push_back(sample);

					//test_set2.push_back(sentence_temp);
				}

		}


		//YOU MAKE THE COMPARE
		//Following call is valid for both methods. This is a global function(gi_utilities.cpp)
		compute_ir_stats(dfa_to_compare,this,test_set,stats1); //this is the target. Samples generated on the target for random walk

		if(method == "random-walk" || useRandomWalk)
		   compute_ir_stats(dfa_to_compare,this,test_set2,stats2);//this is the target. Samples generated on the model, i.e. hypothesis  (dfa_to_compare)

	return useRandomWalk;
}
*/

map<int,string>  Dfa::get_access_strings() const
{

	//// Support structers
	// Record the already visited nodes
	vector<int> 							   	visited_nodes;

	// Queue of nodes to be checked
	list<int>										queue;

	// Structure to record access strings
	map<int,string> 	access_strings;


	//// Init
	// Insert as first state the start state
	queue.push_back(start_state_);
	visited_nodes.push_back(start_state_);
	int current_node = start_state_;


	// BFS
	while(!queue.empty())
	{
		// Reference to the front of the queue
		current_node = queue.front();
		queue.pop_front();

		//cout << "Father node: "<< intTostring(current_node) << endl;


		// Cycle on linked node to current node
		for(string sym : get_alphabet())
		{
			int child_node = get_ttable(current_node,sym);
			//cout << "Child node: "<< intTostring(child_node) << endl;


			// If it is a not visited nodes
			if( std::find(visited_nodes.begin(), visited_nodes.end(), child_node) == visited_nodes.end() )
			{
				// Current access string
				string current_access_string;

				if( access_strings.find(current_node) != access_strings.end()  )
					current_access_string = access_strings[current_node];
				current_access_string=current_access_string+sym;


				// If the entry does not exist in the table, inserts it,
				// otherwise compare lenght, if it is shorter inserts it
				if (access_strings[child_node].empty())
					access_strings[child_node] = current_access_string;
				else if(access_strings[child_node].size() > current_access_string.size() )
					access_strings[child_node] = current_access_string;



				// Insert analyzed child node into queue and visited node set
				visited_nodes.push_back(child_node);
				queue.push_back(child_node);
					//cout << "Nodo "<< intTostring(child_node) << " inserito nella coda"<<endl;
			}
		}

	}

	return access_strings;
}

// Generazione random di samples
// Based on the algorithm described in "STAMINA: a competition to encourage the development
// and assessment of software model inference techniques", by N. Walkinshaw,
// B. Lambeau, C. Damas, K. Bogdanov, P. Dupont. DOI 10.1007/s10664-012-9210-3
map< vector<string>, int> Dfa::generate_pos_neg_samples_without_weights(int n_pos_samples,int n_neg_samples) const
{
	// Final random string: string and type (positive or negative)
	map< vector<string>, int> samples;

	vector<string> alph=get_alphabet();

	srand (time(NULL));

	//int curr_transition_mapped_symbol =0;

	int freq_to_stop = 1+2*(this->get_dim_alphabet());	  // The inverse is probability to stop

	string current_transition_symbol = "";


	bool found_one_minimum_accepted_str = false;
	bool go_next = true;
	int current_state = 0;

	int num_attempts = 0;
	int num_neg_attempts = 0;

	//int debug=1;
	////////////////////////////////////////////////////
	// POSITIVE SAMPLE
	while(samples.size() < n_pos_samples)
	{	
		//cout<<"Iterazione: "<<debug<<endl;
		//debug++;

		// Init
		vector<string> incremental_sample;
		vector<string> positive_sample;


		// New random transition - Input for first transition
		current_transition_symbol = alph[rand() % get_dim_alphabet()];
		//cout<<"dim alph="<<this->get_dim_alphabet()<<endl;

		//cout<<current_transition_symbol<<endl;
		if(current_transition_symbol == ""){
			cerr  << "ERROR: Random transition have no corresponding alphabet simbol" << endl;
			continue;
		}


		// Build a positive sample
		go_next = true;
		current_state = 0;
		int num_iteration = 0;
		bool sink_state = false;
		found_one_minimum_accepted_str = false;

		while(go_next)
		{
			current_state = get_ttable(current_state,current_transition_symbol);

			// Symbol corresponding to transition index
			if(current_transition_symbol == "")
					cerr  << "ERROR: Random transition have no corresponding alphabet_ simbol" << endl;


			// Increment the final random string
			incremental_sample.push_back(current_transition_symbol);

			// Visit DFA with input symbol: if accepting state than stop!
			if(is_accepting(current_state)){
				found_one_minimum_accepted_str = true;
				positive_sample = incremental_sample;
			}


			// Stop condition - // If it stops some good strings should have been generated
			if( ((rand() % freq_to_stop) == 1 && found_one_minimum_accepted_str) || num_iteration > 5000)
				go_next = false;


			// Check if it ended in a sink state
			if(num_iteration > 5000)
				sink_state = true;


			// Generate new input symbol
			current_transition_symbol = alph[rand() % get_dim_alphabet()];


			++num_iteration;
		} // END while(go_next)

		if(num_attempts > ITERATION_LIMIT_FOR_EVERY_GENERAL_PROBLEM){
			cout << "! Poor set of samples ! Actually number of samples: " << samples.size() << endl;
			break;
		}

        //if(positive_sample.at(0).compare(" ") != 0)  //THIS IS ORIGINAL INSTRUCTION
			++num_attempts;

		if(sink_state)
			continue;


		// Record positive sample built
		samples[positive_sample] = 1;

		//cout << "Positive string generated: " << positive_sample << endl;


		

	} // END while(positive_sample)

	//cout << "Actually positive samples: "<< samples.size() << " out of a total of " << n_pos_samples << endl;


	////////////////////////////////////////////////////////////////
	// NEGATIVE SAMPLES
	while(samples.size() - n_pos_samples < n_neg_samples)
	{
		//cout << "Tot: "<<samples.size() <<", Positive: "<<n_pos_samples <<", Negative: "<<n_neg_samples<<endl;

		vector<string> incremental_sample;
		vector<string> positive_sample;

		int mod_type = rand() % 3;			// 0: substituting, 1:inserting, 2:deleting

		int n_of_editing = getPoisson(3);   // Poisson distribution centred on 3
		int changes_done = 0;


		// Read positive samples to be modified
		for(auto curr_sample=samples.begin(); curr_sample!=samples.end(); ++curr_sample)
		{
			// Read a positive sample
			if((*curr_sample).second == 1)
				incremental_sample = (*curr_sample).first;
			//cout << "\nPositive sample read: "<< incremental_sample << endl;

			if(incremental_sample.size() == 0)
				continue;

			// Number of edits to be applied
			n_of_editing = getPoisson(3);


			// Do changes
			changes_done = 0;
			while(changes_done < n_of_editing)									// Make "n_changes" for the single string up to "n_of_editing"
			{

				// Random selection of edits to be applied
				mod_type = rand() % 3;

				// Index of symbol to be modified
				int index_of_substituted_symbol = rand() % incremental_sample.size();


				// 0 - Substitution
				if(mod_type == 0){
					//cout << endl<<"Substitution"<<endl;

					// New symbol to be inserted
					int new_integer_char = rand() % get_dim_alphabet();

					// Map symbol to integer
					string new_symbol = alph[new_integer_char];

					// Substitution with the new symbol
					incremental_sample[index_of_substituted_symbol] = new_symbol;
				}
				// 1 - Insertion
				else if(mod_type == 1){
					//cout <<endl<< "Insertion"<<endl;

					// New symbol to be inserted
					int new_integer_char = rand() % get_dim_alphabet();

					// Map symbol from integer to referred alphabet_ symbol (from int to string)
					string new_symbol = alph[new_integer_char];

					incremental_sample.insert(incremental_sample.begin()+index_of_substituted_symbol, new_symbol);
				}
				// 2 - Removal
				else if(mod_type == 2){
					//cout << endl << "Removal"<<endl;

					// Check if string lenght is less or equal 1
					if(incremental_sample.size() <= 1)
						continue;

					// Remove an element
					incremental_sample.erase(incremental_sample.begin()+index_of_substituted_symbol);
				}

				changes_done++;
			}

			// Check if the new string is a negative one and add it to the set
			// From vector<string> to vector<SYMBOL>
			/*vector<SYMBOL> symbol_incremental_sample(incremental_sample.size());
				i=0;
			    for(auto it=incremental_sample.begin(); it!=incremental_sample.end(); ++it,++i)
			       symbol_incremental_sample.at(i) = mapped_alphabet_.at(*it);
			*/
			if(membership_query(incremental_sample) == 0){
				samples[incremental_sample] = 0;
				//cout << "Rightly rejected string:"<<incremental_sample<<";"<<endl;
			}

            ++num_neg_attempts;

			// Stop when total number of samples have been generated
			if((samples.size() - n_pos_samples >=  n_neg_samples))
				break;
		}
		if( num_neg_attempts>ITERATION_LIMIT_FOR_EVERY_GENERAL_PROBLEM || num_neg_attempts==0 )
		   break;
	}

	return samples;
}

map< vector<string>, int> Dfa::generate_weights_for_pos_neg_samples(map< vector<string>, int> samples, int upper_bound_for_weights)
{
	// Weights of samples: <string, weight>
	map<vector<string>, int> weights;


	// Random weight generation
	srand(time(NULL));
	for(auto it=samples.begin(); it!=samples.end(); ++it)
	{
		int random_weight = rand() % upper_bound_for_weights;
		weights[it->first] = random_weight;
	}

	return weights;
}

void Dfa::print_set_of_pos_neg_samples(map< vector<string>, int> samples, map< vector<string>, int> weights)
{
	// Check if the number of samples and weights is equal
	if(samples.size() != weights.size())
		throw wrongSizes();


	// Print positive sample
	cout << "Positive samples:" << endl;
	for(auto it = samples.begin(), it_weg = weights.begin(); it != samples.end(); ++it, ++it_weg)
	{
		if(it->second == 1){
			cout << "+ ";
			cout << it_weg->second << " ";
			for(auto ch = (it->first).begin(); ch!=(it->first).end(); ++ch)
				cout << (*ch) << " ";
			cout << "\n";
		}
	}


	// Write negative sample
	cout << "Negative samples:" << endl;
	for(auto it = samples.begin(), it_weg = weights.begin(); it != samples.end(); ++it, ++it_weg)
	{
		if(it->second == 0){
			cout << "- ";
			cout << it_weg->second << " ";
			for(auto ch = (it->first).begin(); ch!=(it->first).end(); ++ch)
				cout << (*ch) << " ";
			cout << "\n";
		}
	}
}

bool Dfa::write_existent_set_of_pos_neg_samples_in_file(map< vector<string>, int> samples, map< vector<string>, int> weights, const char * file_path)
{
	// File variable
	ofstream myfile;

	// Write positive and negative samples on file
	//cout << "Writing on: " << file_path << endl;

	myfile.open(file_path);

	myfile << get_dim_alphabet() << "\n";


	// Write alphabet_ symbols
	myfile << "$ ";						// Empty string symbol
	for(int i=0; i<get_dim_alphabet(); ++i)
		myfile << alphabet_[i] <<" ";
	myfile << "\n";


	// Write positive sample
	for(auto it = samples.begin(), it_weg = weights.begin(); it != samples.end(); ++it, ++it_weg)
	{
		if(it->second == 1){
			myfile << "+ ";
			myfile << it_weg->second << " ";
			for(auto ch = (it->first).begin(); ch!=(it->first).end(); ++ch)
				myfile << (*ch) << " ";
			myfile << "\n";
		}
	}


	// Write negative sample
	for(auto it = samples.begin(), it_weg = weights.begin(); it != samples.end(); ++it, ++it_weg)
	{
		if(it->second == 0){
			myfile << "- ";
			myfile << it_weg->second << " ";
			for(auto ch = (it->first).begin(); ch!=(it->first).end(); ++ch)
				myfile << (*ch) << " ";
			myfile << "\n";
		}
	}


	myfile.close();

	return true;
}

bool Dfa::write_existent_set_of_pos_neg_samples_in_file_without_weights(map< vector<string>, int> samples, const char * file_path)
{
	// File variable
	ofstream myfile;

	// Write positive and negative samples on file
	//cout << "Writing on: " << file_path << endl;

	myfile.open(file_path);

	myfile << get_dim_alphabet() << "\n";


	// Write alphabet_ symbols
	myfile << "$ ";						// Empty string symbol
	for(int i=0; i<get_dim_alphabet(); ++i)
		myfile << alphabet_[i] <<" ";
	myfile << "\n";


	// Write positive sample
	for(auto it = samples.begin(); it != samples.end(); ++it)
	{
		if(it->second == 1){
			myfile << "+ ";
			for(auto ch = (it->first).begin(); ch!=(it->first).end(); ++ch)
				myfile << (*ch) << " ";
			myfile << "\n";
		}
	}


	// Write negative sample
	for(auto it = samples.begin(); it != samples.end(); ++it)
	{
		if(it->second == 0){
			myfile << "- ";
			for(auto ch = (it->first).begin(); ch!=(it->first).end(); ++ch)
				myfile << (*ch) << " ";
			myfile << "\n";
		}
	}


	myfile.close();

	return true;
}

// Generazione random di samples
bool Dfa::write_pos_neg_samples_in_file(int n_pos_samples,int n_neg_samples, int upper_bound_for_weights, const char * file_path)
{

	// Final random string: string and type (positive or negative)
	map< vector<string>, int>  samples = generate_pos_neg_samples_without_weights(n_pos_samples, n_neg_samples);


	// Generation of random weights for the samples
	map< vector<string>, int>  weights = generate_weights_for_pos_neg_samples(samples, upper_bound_for_weights);


	// Write in file the generated set of samples
	bool exit_status = write_existent_set_of_pos_neg_samples_in_file( samples, weights, file_path);


	return exit_status;
}

/*
vector<string> Dfa::get_w_method_test_set(Dfa* target_dfa, bool sigma=true) const
{
	vector<string> w_vec;

	set<vector<SYMBOL>> w_set = get_w_method_test_set_mapped_alphabet(target_dfa,sigma);

	for(auto &it1 : w_set) {
		std::vector<char> strvec;
		for (SYMBOL s: it1) {
			char c = get_letter_from_mapped_alphabet(s)[0];
			strvec.push_back(c);
		}
		std::string str(strvec.begin(),strvec.end());
		w_vec.push_back(str);
	}

	return w_vec;
}

set<vector<SYMBOL>> 					Dfa::get_w_method_test_set_mapped_alphabet(Dfa* target_dfa, bool sigma) const
{
	set<vector<SYMBOL>> w_method_test_set;

	#ifdef DEBUG_DFA
	cout << "START Cover set...";
	#endif

	vector<vector<SYMBOL>> cover_set = get_cover_set();
	vector<vector<SYMBOL>> cover_set_target_dfa=target_dfa->get_cover_set();

	#ifdef DEBUG_DFA
	cout << "END. Size: "<< cover_set.size() << endl;
	cout << "START aug char set ... " << flush;
	#endif

	int sigma_exponent=0;
	vector<vector<SYMBOL>> characterization_set;

	if(sigma){

		sigma_exponent=(target_dfa->get_set_depth(cover_set_target_dfa)-get_set_depth(cover_set));

		try
		{
			get_augmented_characterization_set(sigma_exponent, characterization_set);
		}catch(exception &e){
			cerr << "ERR: Too memory allocated" << endl;
			throw ;
		}

	}

	else{	//sigma false so we don't want the central term sigma^k

		try
		{
			characterization_set=get_characterization_set();
		}catch(exception &e){
			cerr << "ERR: Too memory allocated" << endl;
			throw ;
		}

	}
	//here the characterization_set is ready, with or without sigma, as our choice points out

	#ifdef DEBUG_DFA
	cout << "END. Size: " << aug_characterization_set.size() << endl;
	#endif

	// Limit the number of strings
	//if(cover_set.size() * aug_characterization_set.size() > 10000000000)
	//	throw wMethodTestSetTooBig();


	#ifdef DEBUG_DFA
	cout << "START Test set...";
	#endif

	////// Compute test set: concatenating cover set and aug. charac. set
	for(auto &it1 : cover_set)
	{
		// Add non-concatened string of cover set to final set
		//if(it1.size() != 0)
		//	w_method_test_set.insert(it1);


		// Add concatened strings
		for(auto &it2 : characterization_set)
		{
			vector<SYMBOL> tmp_string = it1;

			// Concatenating strings
			tmp_string.insert( tmp_string.end(), it2.begin(), it2.end() );

			// Add string to final set
			w_method_test_set.insert(tmp_string);
		}
	}


	#ifdef DEBUG_DFA
	cout << "END. Size:" << w_method_test_set.size() << endl;
	#endif


	return w_method_test_set;
}
*/