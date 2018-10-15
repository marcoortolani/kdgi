/*
 * concretedfa.cc
 */


#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>	 	//Adopted for issymbol_stream
#include <map>
#include <list>
#include <cmath>
#include <algorithm>
#include <limits>
#include <random>
#include <sys/time.h>
#include <chrono>
#include <ctime>

#include <boost/algorithm/string.hpp>
#include "concretedfa.h"
#include "exceptions.h"
#include "utilities.h"


using namespace std;

#define BUFFER_SIZE 1024

#define INF numeric_limits<int>::max()					/*!< Usually adopted for a non initialized state. */


// For Table Filling Algorithm
#define DFA_TF_STATE_N numeric_limits<short int>::max()		    // Unknown
#define DFA_TF_STATE_X numeric_limits<short int>::max() -1  	// Distinct state (accepting vs rejecting)

// For W-Method
#define LIMIT_OF_TESTSET_W_METHOD 1000000000
#define MAX_DIMENSION_WMETHOD_TEST_SET 500000

// Generation of random symbol_s
#define ITERATION_LIMIT_FOR_EVERY_GENERAL_PROBLEM 6000000


ConcreteDfa::ConcreteDfa():Dfa::Dfa() {
	ttable_ 	= vector<map<symbol_,int>>();
	accepting_states_ = vector<int>();
}

ConcreteDfa::ConcreteDfa(const int n_state, const vector<symbol_> alf, const int s_state):Dfa::Dfa(n_state,alf,s_state){
	for(int i=0; i<n_state; ++i){
		map<symbol_,int> tmp=map<symbol_,int>();
		for(symbol_ sym : alf)
			tmp[sym]=ND;
		ttable_.push_back(tmp);
		accepting_states_.push_back(0);
	}
}


ConcreteDfa::ConcreteDfa(const int n_state, const vector<symbol_> alf)
:ConcreteDfa(n_state, alf, 0){}

ConcreteDfa::ConcreteDfa(const int n_state, const vector<symbol_> alf, const int s_state, vector<map<symbol_,int>> tt_copy, vector<int> accepting_states )
:Dfa::Dfa(n_state, alf, 0){
	ttable_.clear();
	ttable_.reserve(tt_copy.size());
	copy(tt_copy.begin(),tt_copy.end(),back_inserter(ttable_));
	accepting_states_.clear();
	accepting_states_.reserve(accepting_states.size());
	copy(accepting_states.begin(),accepting_states.end(),back_inserter(accepting_states_));
}


ConcreteDfa::ConcreteDfa(const ConcreteDfa &d1)
:ConcreteDfa(d1.num_states_, d1.alphabet_, d1.start_state_, d1.ttable_, d1.accepting_states_){
	update_state_table();
}


ConcreteDfa &ConcreteDfa::operator=(const ConcreteDfa &d1)
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

			update_state_table();
	}

	return *this;
}

bool ConcreteDfa::operator==(const ConcreteDfa &d1) const{
	vector<map<symbol_,int>> ttable_test=get_ttable();
    vector<map<symbol_,int>> ttable_ref=d1.get_ttable();
    bool flag=true;
    int i;
	if(!(equal(d1.alphabet_.begin(), d1.alphabet_.end(), alphabet_.begin()))||start_state_!=d1.start_state_)
		flag=false;
	if(flag){
		for(i=0;i<d1.get_num_states();i++){
			if(d1.is_accepting(i)!=is_accepting(i)){
				flag=false;
				break;
			}
			for(symbol_ sym : d1.alphabet_)
				if(ttable_test[i][sym]!=ttable_ref[i][sym]){
					flag=false;
					break;
				}
			if(!flag)
				break;
		}
	}
	return flag;
}


bool ConcreteDfa::operator<(const ConcreteDfa &d1) const
{
	return this->num_states_ < d1.num_states_;
}


ConcreteDfa::~ConcreteDfa(){
	alphabet_.clear();
	ttable_.clear();
}


void ConcreteDfa::set_ttable(const vector<map<symbol_,int>> ext_ttable){
	ttable_.clear();
	ttable_.reserve(ext_ttable.size());
	copy(ext_ttable.begin(),ext_ttable.end(),back_inserter(ttable_));
}


ConcreteDfa ConcreteDfa::read_dfa_file(const string file_name)
{
	char nameDFA[BUFFER_SIZE];
	char line[BUFFER_SIZE];

	vector<symbol_> alphabet_file;

	int counter = 0;
	int num_total_line = 0;
	int cstato = 0;
	char calfabeto[30];
	int ctransizione = 0;
	int current_line = 0;
	int header_line = 0;
	int start_state_ = 0;

	symbol_ n;
	symbol_ accepting_symbol;

	ifstream read;
	symbol_ template_line;

	ConcreteDfa res;

	// Open connection to file
	read.open(file_name.c_str());

	if(read.is_open()){
		#ifdef VERBOSE
		cout << "File " << file_name << " is open."<< endl << endl;
		#endif
	}
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

	//ok
	//cout<<"dim alph:"<<dim_alphabet<<endl;

	// Check if the first line is complete
	if(counter != 3){
		cout << "Error in first line of file" << endl;
		throw invalidFormat();
	}
	header_line += 1;


	// read the alphabet_
	alphabet_file.reserve(dim_alphabet);
	read.getline(line,BUFFER_SIZE);

	istringstream iss(line);

	counter=0;
	while (iss >> n){
		if(counter >= dim_alphabet)
			break;

		alphabet_file.push_back(n);
		counter++;
	}

	// check read alphabet_
	if(counter != dim_alphabet){
		cout << "Error in second line of file: issue with size of alphabet_" << endl;

		//if(alphabet_file)
		//	alphabet_file.clear();

		throw wrongAlphabetSize();
	}

	header_line += 1;


	// Set alphabet_ for the current dfa
	res.set_alphabet(alphabet_file);

	// Third line contains the index of the start node
	read.getline(line,BUFFER_SIZE);
	counter = sscanf(line, "%d", &(res.start_state_));
	if(res.start_state_ >= res.num_states_){
		cerr << "ERROR in third line of file: start_state is not a state of dfa. i.e. start_state >= num_states"<<endl;
	}
	header_line += 1;

	// Fourth line contains the symbol to denote the accepting state
	read.getline(line,BUFFER_SIZE);
	istringstream iss1(line);
	iss1 >> n;
	accepting_symbol = n;

	header_line += 1;
	
	//if(alphabet_file)
	//	alphabet_file.clear();


	///////////// compute utility values//////////////////
	// expected lines of file
	num_total_line = (dim_alphabet+1)*(res.num_states_);

	// template of the all lines of the file
	template_line = (symbol_)nameDFA+"[%d][%[^]]] = %d;";
	/////////////////////////////////////////////////////

	// allocate memory for ttable_
	res.ttable_.reserve(res.num_states_*alphabet_file.size());;

	// "+2" is for algorithm like EDSM with states Type and Colour
	res.accepting_states_.reserve(res.num_states_);
	for(int i=0; i<res.num_states_; ++i){
		map<symbol_,int> tmp=map<symbol_,int>();
		for(symbol_ sym : alphabet_file)
			tmp[sym]=ND;
		res.accepting_states_.push_back(0);	
		res.ttable_.push_back(tmp);
	}

	// Parsing the file
	while(!read.eof())
	{
		read.getline(line,BUFFER_SIZE);

		symbol_ cline = line;

		// Handler for last line
		symbol_ trimmedline = trim(cline);
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
		symbol_ transition_symbol = calfabeto;

		//cout<<"cstato="<<cstato<<" transition_symbol="<<transition_symbol<<" ctransizione="<<ctransizione<<endl;
		res.ttable_[cstato][transition_symbol] = ctransizione;
		//cout<<"res.ttable_[cstato][transition_symbol]="<<res.ttable_[cstato][transition_symbol]<<endl;
		//res.set_ttable_entry(cstato,transition_symbol,ctransizione);
		// It detects the row for type of state (accepting or rejecting)
		if(transition_symbol.compare(accepting_symbol) == 0)
			res.accepting_states_[cstato]=ctransizione;
			//res.set_accepting_state(cstato);

	}

	if(read.eof() && (current_line+header_line)!=(num_total_line+4)){
		cerr << "ERROR in current Dfa file"<<endl<<endl << "Correct encoding is:"<<endl <<"*********************"<<endl<<endl<<"dim_alphabet	num_states	dfa"<<endl<<"alphabet_symbols"<<endl<<"start_state"<<endl<<"accepting/rejecting_state_marker"<<endl<<"<list_of_transitions written like dfa[state][symbol]=arrive_state>"<<endl<<endl<<"*********************"<<endl<<endl;
	}

	// Close connection
	read.close();

	return res;
}

const vector<int> ConcreteDfa::get_accepting_states() const{
	return accepting_states_;
}

bool ConcreteDfa::is_accepting(int curr_state) const{
	if(accepting_states_[curr_state]==DFA_STATE_ACCEPTING)
		return true;
	else
	return false;
}

vector<map<symbol_,int>> ConcreteDfa::get_ttable() const {
	return ttable_;
}

int ConcreteDfa::get_ttable(int i, symbol_ j) const {
	vector<symbol_> alph=get_alphabet();
	if(i<num_states_ && std::find(alph.begin(), alph.end(), j) != alph.end())
		return ttable_[i].at(j);
	else{
		cout<<"errore i="<<i<<"symbol_ j="<<j<<endl;
		cerr<<"dfa::get_ttable: out of bound"<<endl;
		throw indexOutOfBoundTtable();
	}
}

void ConcreteDfa::set_ttable_entry(int i, symbol_ j, int v){
	vector<symbol_> alph=get_alphabet();
	if(i<num_states_ && std::find(alph.begin(), alph.end(), j) != alph.end())
		ttable_[i][j]=v;
	else{
			cerr<<"dfa::set_ttable_: out of bound"<<endl;
			throw indexOutOfBoundTtable();
		}
}

void ConcreteDfa::set_accepting_states_entry(int state, int value){
	accepting_states_[state] = value;
}

void ConcreteDfa::set_accepting_state(int state_to_mark){
		accepting_states_[state_to_mark]=1;
}

void ConcreteDfa::set_rejecting_state(int state_to_mark){
	if(state_to_mark<get_dim_alphabet())
		accepting_states_[state_to_mark]=0;
}

void ConcreteDfa::set_ttable_from_sequence(const vector<int> &sequence)
{
	vector<symbol_> alph = get_alphabet();
	int n_col = alph.size()+1;
	if((sequence.size() % n_col) != 0 || (sequence.size() / n_col) != num_states_)
	{
		cerr << "Impossible set the ttable_.\nDimension of sequence isn't a multiple of dimension of alphabet_ +1\n or the sequence doesn't correspond to number of states " << endl;
		throw invalidParameters();
	}

	for(int i=0; i<num_states_; ++i){
		int cnt=0;
		for(symbol_ sym : alph){
			ttable_[i][sym] = sequence[(i*n_col)+cnt];
			++cnt;
		}
		if(sequence[i*(alph.size()+1)+alph.size()])
			accepting_states_[i]=1;
	}
}

symbol_ ConcreteDfa::random_ttable()
{
	symbol_ random_sequence;
	int extracted_number;
	random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, get_num_states()-1);
    uniform_int_distribution<> dis2(0, 1); //for accepting/rejecting column

	//cout<<"The extracted random ttable_ is :"<<endl;
	for(int row=0; row<get_num_states(); ++row)
	{
		for(symbol_ sym : get_alphabet())
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

ConcreteDfa* ConcreteDfa::minimize_TF() const
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

					for(symbol_ sym : get_alphabet())
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
	ConcreteDfa* dfa_min = new ConcreteDfa(final_states_counter, alphabet_, start_state_);

	int count = 0;
	for(int i=0; i<num_states_; ++i){
		if(equivalent_state[i] == ND){
			for(symbol_ sym : get_alphabet())
				dfa_min->set_ttable_entry(count,sym,get_ttable(i,sym));
			count++;
		}
		dfa_min->set_accepting_states_entry(i,get_accepting_states()[i]);
	}


	// Old transitions towards deleteted states are updated towards the new equivalent state
	for(int i=0; i<num_states_; ++i){
		if(equivalent_state[i] != ND)
		{
			for(int k=0; k<final_states_counter; ++k)
				for(symbol_ sym : get_alphabet())
					// Transition toward "i" state is substitutes with one towards equivalent state "equivalent_state[i]"
					if(dfa_min->get_ttable(k,sym) == i)
						dfa_min->set_ttable_entry(k,sym,equivalent_state[i]);
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
				for(symbol_ sym : get_alphabet())
					if(dfa_min->get_ttable(k,sym) == i)
						dfa_min->set_ttable_entry(k,sym,nuova_label);
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

void ConcreteDfa::print_dfa_ttable(symbol_ title) const
{
	// It uses Mapped alphabet_


	// Print an header
	cout << endl<< "--------------------------" << endl;
	cout << title << endl;
	symbol_ header = "  ";
	for(int i=0; i<get_dim_alphabet(); ++i)
		header = header + " | "+ alphabet_[i];
	header = header + " - A";
	cout << header << endl;


	// Print transitions
	for(int i=0; i<num_states_; ++i){
		cout << "S"<<i<<"  ";

		for(symbol_ sym : get_alphabet())
		{
			// Transition values: a value or "ND"
			if(get_ttable(i,sym) == ND)
				cout << " N ";
			else
				cout << " "<< get_ttable(i,sym) <<"  ";
		}
		// Type of state
		if(accepting_states_[i] == DFA_STATE_NON_ACCEPTING )
			cout << "  / ";
		else if(accepting_states_[i] == DFA_STATE_ACCEPTING)
			cout << " Ac ";
		else if(accepting_states_[i] == DFA_STATE_REJECTING)
			cout << " Ri ";
		else
		cout << "  X ";

		cout << endl;
	}

	cout << "--------------------------" << endl;
}

void ConcreteDfa::print_dfa_dot(string title, const char *file_path)
{
	ofstream myfile;
	myfile.open(file_path);


	// Initialization of symbol_s with DOT code
	symbol_ header = "digraph "+title+" {\n";
	symbol_ start_state_ = "__start0 [label=\"\" shape=\"none\"];\n\n";

	start_state_ = start_state_ + "rankdir=LR;\nsize=\"8,5\";\n\n";


	//States
	symbol_ states = "";
	symbol_ shape = "";
	symbol_ style="";
	symbol_ color="";
	for(int i=0; i<num_states_; ++i)
	{
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
	symbol_ transitions = "";
	for(int i=0; i<num_states_; ++i){
		for(symbol_ sym : get_alphabet()){
			int arrive_state = get_ttable(i,sym);
			if(arrive_state == ND)
				continue;

			transitions = transitions + "s"+std::to_string(i)+" -> s"+std::to_string(arrive_state)+" [label=\""+sym+"\"];\n";
		}
	}


	symbol_ end = "__start0 -> 0;";
	symbol_ footer ="\n}";


	// Finally, it prints overall DOT code
	myfile << header << start_state_ << states << transitions << footer;

	myfile.close();
}

void ConcreteDfa::print_dfa_in_text_file(const symbol_ file_path)
{

	// Check if some transitions is "Undefined"
	for(int i=0; i<num_states_; ++i)
		for(symbol_ sym : get_alphabet())
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
	for(symbol_ sym : get_alphabet())
		myfile << sym << " ";
	myfile << "\n";

	// Write start_state
	myfile << start_state_ << "\n";

	// Write accepting symbol
	myfile << "@" << "\n";


	// Write transition table
	for(int i=0; i<num_states_; ++i){
		for(symbol_ sym : get_alphabet())
		{
			myfile << "dfa[" <<std::to_string(i)<<"][";
			
			myfile << sym << "]="<< get_ttable(i,sym) <<";\n";
		}
		myfile << "dfa["<<std::to_string(i)<<"][";
		myfile << "@" << "]=";
		if(is_accepting(i))
			myfile << "1";
		else 
			myfile << "0";

		myfile <<";\n";
	}

	myfile.close();
}

int ConcreteDfa::get_arrive_state(vector<symbol_> &phrase) const
{
	int state = get_start_state();
	int next_state=ND;

	for(symbol_ sym : phrase){
		next_state = get_ttable(state,sym);
		if(next_state == ND){
			state = ND;
			break;
		}
		state = next_state;
	}
	// It returns "ND" if the provided symbol_ is not compatible, otherwise it returns reached state
	return state;
}

bool ConcreteDfa::membership_query(vector<symbol_> phrase)const{

	// Check if arrive_state is ND (for DFA without sink state)
	int arrive_state = get_arrive_state(phrase);
	if(arrive_state == ND)
		return false;

	if(is_accepting(arrive_state))
		return true;
	else
		return false;
}


bool ConcreteDfa::compare_dfa( ConcreteDfa *dfa_to_compare , symbol_ method , ir_statistical_measures &stats1 , ir_statistical_measures &stats2 )
{
	boost::algorithm::to_lower(method);
	if(method != "w-method" && method != "random-walk")
	{
	   cerr<<"Pointed out method to use for comparison is wrong"<<endl;
	   throw invalidParameters();
	}

	vector<vector<symbol_>> test_set;
	vector<vector<symbol_>> test_set2;
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
				//vector<symbol_> sentence_temp;

				//convert the structure returned from random walk in a set<vector<symbol_>>
				for(auto &samples_set : this->generate_pos_neg_samples_without_weights(num_pos_samples,num_neg_samples) )
				{
					//sentence_temp = {};
					//for(auto & sample : samples_set.first) //samples_set.first is a vector<symbol_>
						//test_set.push_back(sample);
					test_set.push_back(samples_set.first);
					//test_set.push_back(sentence_temp);
				}

				for(auto &samples_set : dfa_to_compare->generate_pos_neg_samples_without_weights(num_pos_samples,num_neg_samples) )
				{
					//sentence_temp = {};
					//for(auto & sample : samples_set.first) //samples_set.first is a vector<symbol_>
						//test_set2.push_back(sample);
					test_set2.push_back(samples_set.first);
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


map<int,vector<symbol_>>  ConcreteDfa::get_access_strings() const
{

	//// Support structers
	// Record the already visited nodes
	vector<int> 							   	visited_nodes;

	// Queue of nodes to be checked
	list<int>										queue;

	// Structure to record access symbol_s
	map<int,vector<symbol_>> 	access_symbol_s;


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

		//cout << "Father node: "<< intTosymbol_(current_node) << endl;


		// Cycle on linked node to current node
		for(symbol_ sym : get_alphabet())
		{
			int child_node = get_ttable(current_node,sym);
			//cout << "Child node: "<< intTosymbol_(child_node) << endl;


			// If it is a not visited nodes
			if( std::find(visited_nodes.begin(), visited_nodes.end(), child_node) == visited_nodes.end() )
			{
				// Current access symbol_
				vector<symbol_> current_access_symbol_;

				if( access_symbol_s.find(current_node) != access_symbol_s.end()  )
					current_access_symbol_ = access_symbol_s[current_node];
				current_access_symbol_.push_back(sym);


				// If the entry does not exist in the table, inserts it,
				// otherwise compare lenght, if it is shorter inserts it
				if (access_symbol_s[child_node].empty())
					access_symbol_s[child_node] = current_access_symbol_;
				else if(access_symbol_s[child_node].size() > current_access_symbol_.size() )
					access_symbol_s[child_node] = current_access_symbol_;



				// Insert analyzed child node into queue and visited node set
				visited_nodes.push_back(child_node);
				queue.push_back(child_node);
					//cout << "Nodo "<< intTosymbol_(child_node) << " inserito nella coda"<<endl;
			}
		}

	}

	return access_symbol_s;
}

// Generazione random di samples
// Based on the algorithm described in "STAMINA: a competition to encourage the development
// and assessment of software model inference techniques", by N. Walkinshaw,
// B. Lambeau, C. Damas, K. Bogdanov, P. Dupont. DOI 10.1007/s10664-012-9210-3
map< vector<symbol_>, int> ConcreteDfa::generate_pos_neg_samples_without_weights(int n_pos_samples,int n_neg_samples) const
{
	// Final random symbol_: symbol_ and type (positive or negative)
	map< vector<symbol_>, int> samples;

	vector<symbol_> alph=get_alphabet();

	srand (time(NULL));

	//int curr_transition_mapped_symbol =0;

	int freq_to_stop = 1+2*(this->get_dim_alphabet());	  // The inverse is probability to stop

	symbol_ current_transition_symbol = "";


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
		vector<symbol_> incremental_sample;
		vector<symbol_> positive_sample;


		// New random transition - Input for first transition
		current_transition_symbol = alph[rand() % get_dim_alphabet()];
		//cout<<"dim alph="<<this->get_dim_alphabet()<<endl;

		//cout<<current_transition_symbol<<endl;
		if(current_transition_symbol == ""){
			#ifdef VERBOSE
			cerr  << "ERROR: Random transition have no corresponding alphabet simbol" << endl;
			#endif
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
			#ifdef VERBOSE
			if(current_transition_symbol == "")
				cerr  << "ERROR: Random transition have no corresponding alphabet_ simbol" << endl;
			#endif

			// Increment the final random symbol_
			incremental_sample.push_back(current_transition_symbol);

			// Visit DFA with input symbol: if accepting state than stop!
			if(is_accepting(current_state)){
				found_one_minimum_accepted_str = true;
				positive_sample = incremental_sample;
			}


			// Stop condition - // If it stops some good symbol_s should have been generated
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

		//cout << "Positive symbol_ generated: " << positive_sample << endl;


		

	} // END while(positive_sample)

	//cout << "Actually positive samples: "<< samples.size() << " out of a total of " << n_pos_samples << endl;


	////////////////////////////////////////////////////////////////
	// NEGATIVE SAMPLES
	while(samples.size() - n_pos_samples < n_neg_samples)
	{
		//cout << "Tot: "<<samples.size() <<", Positive: "<<n_pos_samples <<", Negative: "<<n_neg_samples<<endl;

		vector<symbol_> incremental_sample;
		vector<symbol_> positive_sample;

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
			while(changes_done < n_of_editing)									// Make "n_changes" for the single symbol_ up to "n_of_editing"
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
					symbol_ new_symbol = alph[new_integer_char];

					// Substitution with the new symbol
					incremental_sample[index_of_substituted_symbol] = new_symbol;
				}
				// 1 - Insertion
				else if(mod_type == 1){
					//cout <<endl<< "Insertion"<<endl;

					// New symbol to be inserted
					int new_integer_char = rand() % get_dim_alphabet();

					// Map symbol from integer to referred alphabet_ symbol (from int to symbol_)
					symbol_ new_symbol = alph[new_integer_char];

					incremental_sample.insert(incremental_sample.begin()+index_of_substituted_symbol, new_symbol);
				}
				// 2 - Removal
				else if(mod_type == 2){
					//cout << endl << "Removal"<<endl;

					// Check if symbol_ lenght is less or equal 1
					if(incremental_sample.size() <= 1)
						continue;

					// Remove an element
					incremental_sample.erase(incremental_sample.begin()+index_of_substituted_symbol);
				}

				changes_done++;
			}

			if(membership_query(incremental_sample) == 0){
				samples[incremental_sample] = 0;
				//cout << "Rightly rejected symbol_:"<<incremental_sample<<";"<<endl;
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

map< vector<symbol_>, int> ConcreteDfa::generate_weights_for_pos_neg_samples(map< vector<symbol_>, int> samples, int upper_bound_for_weights)
{
	// Weights of samples: <symbol_, weight>
	map<vector<symbol_>, int> weights;


	// Random weight generation
	srand(time(NULL));
	for(auto it=samples.begin(); it!=samples.end(); ++it)
	{
		int random_weight = rand() % upper_bound_for_weights;
		weights[it->first] = random_weight;
	}

	return weights;
}

void ConcreteDfa::print_set_of_pos_neg_samples(map< vector<symbol_>, int> samples, map< vector<symbol_>, int> weights)
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

bool ConcreteDfa::write_existent_set_of_pos_neg_samples_in_file(map< vector<symbol_>, int> samples, map< vector<symbol_>, int> weights, const char * file_path)
{
	// File variable
	ofstream myfile;

	// Write positive and negative samples on file
	//cout << "Writing on: " << file_path << endl;

	myfile.open(file_path);

	myfile << get_dim_alphabet() << " w" << "\n";


	// Write alphabet_ symbols
	myfile << "$ ";						// Empty symbol_ symbol
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

bool ConcreteDfa::write_existent_set_of_pos_neg_samples_in_file_without_weights(map< vector<symbol_>, int> samples, const char * file_path)
{
	// File variable
	ofstream myfile;

	// Write positive and negative samples on file
	//cout << "Writing on: " << file_path << endl;

	myfile.open(file_path);

	myfile << get_dim_alphabet() << "\n";


	// Write alphabet_ symbols
	myfile << "$ ";						// Empty symbol_ symbol
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
bool ConcreteDfa::write_pos_neg_samples_in_file(int n_pos_samples,int n_neg_samples, int upper_bound_for_weights, const char * file_path)
{

	// Final random symbol_: symbol_ and type (positive or negative)
	map< vector<symbol_>, int>  samples = generate_pos_neg_samples_without_weights(n_pos_samples, n_neg_samples);


	// Generation of random weights for the samples
	map< vector<symbol_>, int>  weights = generate_weights_for_pos_neg_samples(samples, upper_bound_for_weights);


	// Write in file the generated set of samples
	bool exit_status = write_existent_set_of_pos_neg_samples_in_file( samples, weights, file_path);


	return exit_status;
}

/*bool ConcreteDfa::equivalence_query(ConcreteDfa* dfa_hp,vector<symbol_> *witness_results) {

    bool areEquivalent;

	#ifdef DEBUG_DFA
		cout << endl << "--------------------------" << endl;
		cout << "EQUIVALENCE QUERY" << endl;
		cout << "--------------------------" << endl;
	#endif

	// Build union DFA of target dfa (thisone) and dfa_hp
	ConcreteDfa* dfa_union = this->unionDFA(dfa_hp);

	#ifdef DEBUG_2
		dfa_union->print_dfa_ttable_("DFA UNIONE");
	#endif


	// Table-filling algorithm on union dfa
	vector<symbol_> distincts_table = dfa_union->table_filling();

	// Extract list of equivalent states from table of distinct states,
	// every vector contain a list of equivalent states for the state that correspond to the vector.
	vector<vector<int>> equivalent_states_list = dfa_union->equivalent_states_list_from_table(distincts_table);
	
	int start_state = get_start_state();
	int hp_start_state = dfa_hp->get_start_state();


	// Checks if start states of dfas are equivalent:
	// check if among the set of states equivalent to the 0 state (start state of current automaton)
	// there is the start state of hp automaton, whose index is "num_state"
	// If so, it returns an empty vector as counterexample.
	if(equivalent_states_list[start_state].end() == find(equivalent_states_list[start_state].begin(), equivalent_states_list[start_state].end(), hp_start_state+num_states_) )
	{
		areEquivalent = false;
		if(witness_results!=NULL) //if witness_results is NULL means that the client isn't interested in witness but in checking only equivalence
		   *witness_results = dfa_union->witness_from_table(distincts_table, dfa_hp->get_start_state()+num_states_);
	}
	else //The dfa are equivalentes
	   areEquivalent = true;

	// Free allocated memory
	/*
	if(distincts_table != NULL)
		delete[] distincts_table;

	delete [] equivalent_states_list;
	*/

	//delete dfa_union;

	//return areEquivalent;
//}

// It returns a table saved in a linear array, with a list of equivalent/different states; if it is needed it returns also a counterexample symbol_
vector<symbol_> ConcreteDfa::table_filling() const{
	// The Table considered is only the upper triangular matrix, that can be saved in a linear array of size n(n-1)/2
	// Conversion of index form matrix to array are:
	//
	// From linear index k, to (i,j) for tha matrix (i row, j column)
	// i = n - 2 - floor(sqrt(-8*k + 4*n*(n-1)-7)/2.0 - 0.5)
	// j = k + i + 1 - n*(n-1)/2 + (n-i)*((n-i)-1)/2
	//
	// From (i,j) to k
	// Order such that i<j, then:
	// k = (n*(n-1)/2) - (n-i)*((n-i)-1)/2 + j - i - 1
	//
	// check (http://stackoverflow.com/questions/27086195/linear-index-upper-triangular-matrix)


	// *** TABLE-FILLING ALGORITHM with witness ***
	int i,j,k;
	int n = num_states_;
	int tf_l = (num_states_*(num_states_-1))/2;


	// Table of distinct pair states.
	vector<symbol_> table_of_distinct_states;
	for(int i=0; i<tf_l;++i)	table_of_distinct_states.push_back("");

	// Acceptor and rejector states are surely different, so they are marked.
	for(i=0; i<(num_states_-1); ++i)
		for(j=i+1; j<num_states_; ++j){
			k= (n*(n-1)/2) - (n-i)*((n-i)-1)/2 + j - i - 1;
			// Check if one state is acceptor and another is rejector
			if(is_accepting(i) != is_accepting(j)){
				table_of_distinct_states[k] = to_string(DFA_TF_STATE_X);
			}else
				table_of_distinct_states[k]=  to_string(DFA_TF_STATE_N);
		}


	// Minimizing loop
	// Check at each iteration if the table was modified
	bool modified = true;
	while(modified)
	{
		modified = false;
		int arrive_state_1;
		int arrive_state_2;

		for(i=0; i<(num_states_-1); ++i){
			for(j=i+1; j<num_states_; ++j){

				k = (n*(n-1)/2) - (n-i)*((n-i)-1)/2 + j - i - 1;

				if(table_of_distinct_states[k] == to_string(DFA_TF_STATE_N)){

					for(symbol_ sym : get_alphabet())
					{
						arrive_state_1 = get_ttable(i,sym);
						arrive_state_2 = get_ttable(j,sym);


						// If the arrive state of DFA is a self-pair (e.g. (2,2)), it continues to the next one
						if(arrive_state_1 == arrive_state_2)
							continue;


						// By definition, in the table of pair states turns out that j>i always,
						// then pairs shuould have i<j
						if(arrive_state_2 < arrive_state_1){
							int tmp = arrive_state_1;
							arrive_state_1 = arrive_state_2;
							arrive_state_2 = tmp;
						}


						// If arrive_state is the same state at the beginning, it continues to next one
						if(arrive_state_1 == i && arrive_state_2 == j)
							continue;


						// If arrive pair of states is distinct, then is start state pair as well (!)
						int i1 = arrive_state_1, j1 = arrive_state_2;
						int k1 = (n*(n-1)/2) - (n-i1)*((n-i1)-1)/2 + j1 - i1 - 1;


						if(table_of_distinct_states[k1] != to_string(DFA_TF_STATE_N)){
							table_of_distinct_states[k] = sym;
							modified = true;
							break;
						}
					}
				}
			}
		}
	}


	// Debug print: it prints the table of pair states
	//    '@'  	-->	 It is an equivalent pair of states because its cell in the table remained empty
	// 	  'X'  	-->  It is a distinct pair of states
	// Otherwise ->	 It prints the symbol distincting the pair of states starting from initial state
	#ifdef DEBUG_DFA
	cout << "--------------------------" << endl;
	cout << "Table of pairs " << endl;
	cout << "--------------------------" << endl;
	for(i=0; i<(num_states_-1); ++i){
		for(j=i+1; j<num_states_; ++j){
			k=(n*(n-1)/2) - (n-i)*((n-i)-1)/2 + j - i - 1;
			char toprint= (table_of_distinct_states[k]==DFA_TF_STATE_X)?'X':(table_of_distinct_states[k]==DFA_TF_STATE_N)?'@': (char)(table_of_distinct_states[k] + 48);
			cout << "("<< i << "," << j << "):" << toprint << "  ";
		}
		cout << endl;
	}
	cout << "--------------------------" << endl;
	#endif


	return table_of_distinct_states;
}

ConcreteDfa* ConcreteDfa::unionDFA(ConcreteDfa* dfa_hp) const{
	int count_state = dfa_hp->num_states_ + num_states_;

	// Instance of union dfa
	ConcreteDfa* union_dfa = new ConcreteDfa(count_state, alphabet_, start_state_);

	// Configuration of Union DFA
	// Smaller indexes are given to target dfa, while others to hypothesis
	for(int j=0; j<num_states_; ++j)									// Target automaton
		for(symbol_ sym : alphabet_)
			union_dfa->set_ttable_entry(j,sym,get_ttable(j,sym));

	for(int j=0; j<dfa_hp->num_states_; ++j)						// Hypothesis automaton
		for(symbol_ sym_hp : alphabet_)					// In union dfa, start state of HP dfa is recorded in "num_state" index of target dfa
			union_dfa->set_ttable_entry(num_states_+j,sym_hp, (dfa_hp->get_ttable(j,sym_hp) + num_states_));

	for(int i=0; i<num_states_;++i)
		if(is_accepting(i))
			union_dfa->accepting_states_[i]=1;
	for(int j=0; j<dfa_hp->num_states_;++j)
		if(dfa_hp->is_accepting(j))
			union_dfa->accepting_states_[j+num_states_]=1;

	return union_dfa;
}

vector<vector<int>> ConcreteDfa::equivalent_states_list_from_table(vector<symbol_> distincts)
{
	#ifdef DEBUG_DFA
	cout << endl << "--------------------------" << endl;
	cout << "List of equivalent states:" << endl;
	cout << "--------------------------" << endl;
	#endif


	int n = num_states_;
	vector<vector<int>> equivalent_states;
	for(int i=0;i<(num_states_-1);++i)
		equivalent_states.push_back(vector<int>());


	for(int i=0; i<(num_states_-1); ++i)
		for(int j=i+1; j<num_states_; ++j){
			int k= (n*(n-1)/2) - (n-i)*((n-i)-1)/2 + j - i - 1;
			if(distincts[k] == to_string(DFA_TF_STATE_N))
				equivalent_states[i].push_back(j);
		}


	#ifdef DEBUG_DFA
	cout << "Number of confirmed states: " << num_states_ << endl;
	for(int i=0; i<num_states_; ++i)
		cout << "S["<<i<<"] --> "<< equivalent_states[i] << endl;

	cout << "--------------------------" << endl;
	#endif

	return equivalent_states;
}

vector<symbol_> ConcreteDfa::witness_from_table(vector<symbol_> distinct, int start_state__dfa_hp)
{
	// If automata are not equivalent, it generates a witness symbol_
	vector<symbol_> wit = {};

	int i_pair = get_start_state();
	int j_pair = start_state__dfa_hp;

	symbol_ input;

	#ifdef DEBUG_DFA
	cout << "--- Make a counterexmple --- " << endl;
	#endif


	while(1)
	{
		int n = num_states_;
		int k = (n*(n-1)/2) - (n-i_pair)*((n-i_pair)-1)/2 + j_pair - i_pair - 1;


		// Check if provided automata are equivalent
		if(distinct[k] == to_string(DFA_TF_STATE_N)){
			cerr << "ERR: a witness symbol_ was requested while automata are equivalent!";
			throw witnessFromEquivalentDFA();
		}


		input = distinct[k];


		// Check if one start state is acceptor and the other one is rejector
		if(input == to_string(DFA_TF_STATE_X)) {
			break;
		}


		wit.push_back(input);


		i_pair = ttable_[i_pair][input];
		j_pair = ttable_[j_pair][input];


		if(distinct[k] == to_string(DFA_TF_STATE_X))
			break;

	}

	#ifdef DEBUG_DFA
		cout << "Counterexample is: "<< wit << endl;
	#endif

	return wit;
}

size_t ConcreteDfa::get_set_depth(vector<vector<symbol_> > set) const{
	size_t max=0;
	for(vector<symbol_> phrase : set)
		if(phrase.size()>max)
			max=phrase.size();
  	
  return max;
}

vector<vector<symbol_ >> ConcreteDfa::get_cover_set() const
{

	//// Utility structures
	// Record the already visited nodes
	vector<int> 							   	visited_nodes;

	// Queue of nodes to be checked
	list<int>										queue;

	// Structure to record access symbol_s
	map<int, vector<symbol_>> 	access_symbol_s;

	// Cover set
	vector<vector<symbol_>> 		cover_set(1, vector<symbol_>());  	// "1" is for epsilon transition


	//// Init
	// Insert as first state the start state
	queue.push_back(start_state_);
	visited_nodes.push_back(start_state_);
	int current_node = start_state_;


	//// BFS
	while(!queue.empty())
	{
		// Reference to the front of the queue
		current_node = queue.front();
		queue.pop_front();


		//cout << "Father node: "<< intTosymbol_(current_node) << endl;


		// Cycle on linked node to current node
		for(symbol_ sym : get_alphabet())
		{
			int child_node = get_ttable(current_node,sym);
						//cout << "Child node: "<< intTosymbol_(child_node) << endl;


			// Add symbol_ for transition towards child nodes
			vector<symbol_> child_access_symbol_ = access_symbol_s[current_node];
			//cout<<access_symbol_s[current_node];
			child_access_symbol_.push_back(sym);


			// If it is a not visited node
			if( std::find(visited_nodes.begin(), visited_nodes.end(), child_node) == visited_nodes.end() )
			{
				// Current access symbol_
				vector<symbol_> current_access_symbol_;

				if( access_symbol_s.find(current_node) != access_symbol_s.end()  )
					current_access_symbol_ = access_symbol_s[current_node];
				current_access_symbol_.push_back(sym);
						//cout << "String for father: "<<access_symbol_s[current_node] << endl;
						//cout << "Current symbol_: "<< current_access_symbol_ << endl;


				// If no entry was in the table, one is added. Otherwise, it compares lenghts, the shorter will be recorded
				if (access_symbol_s[child_node].empty())
					access_symbol_s[child_node] = current_access_symbol_;
				else if(access_symbol_s[child_node].size() > current_access_symbol_.size() )
					access_symbol_s[child_node] = current_access_symbol_;


				// Insert analyzed child node into queue and visited node set
				visited_nodes.push_back(child_node);
				queue.push_back(child_node);
				cover_set.push_back(child_access_symbol_);
						//cout << "Node "<< intTosymbol_(child_node) << " added to queue" << endl;
			}
		}

	}

	return cover_set;
}

vector<vector<symbol_> > ConcreteDfa::get_characterization_set() const{
	// Characterization set of examples for current DFA
	vector<vector<symbol_> > characterization_set(0, vector<symbol_>());


	// Table-filling algorithm over union dfa
	vector<symbol_> distincts_table = this->table_filling();


	// Extract list of equivalent states from table of distinct states,
	// every vector contain a list of equivalent states for the state that correspond to the vector.
	// vector<int>* equivalent_states_list = this->equivalent_states_list_from_table(distincts_table);

	#ifndef DEBUG_OBPA //not exec this control for OBP APPROXIMATED
	// Check if the curret automaton is minimal
	ConcreteDfa* tmp_minimized_dfa = this->minimize_TF();
	if(tmp_minimized_dfa->get_num_states() != num_states_){
		cout << "ERROR: Processed DFA is not minimial! Minimize it" << endl;
		delete tmp_minimized_dfa;
		throw mandatoryMinimalDFA();
	}
	delete tmp_minimized_dfa;
	#endif

	// Track pairs of states already checked
	map<int, vector<int>> state_pairs;


	// For each pair of states, a symbol_ must be defined exploiting the symbols within the table of Table-filling
	// These symbols makes a difference between the analyzed states.
	for(int i=0; i<num_states_; ++i)
	{
		for(int j=i+1; j<num_states_; ++j)
		{
			//cout<<"sono in : ("<<i<<","<<j<<")"<<endl;
			// If current pair was checked yet, it goes over
			if( std::find(state_pairs[i].begin(), state_pairs[i].end(), j) != state_pairs[i].end() ){
				//cout<<"coppia già visitata"<<endl;
				continue;
			}
			// Generated witness for the pairs of states
			vector<symbol_> wit;

			// Read symbol during the execution
			symbol_ read_symbols;

			int i_pair = i;
			int j_pair = j;


			while(1)
			{

				// By definition, in the table of pair states turns out that j>i always,
				// then pairs shuould have i<j
				if(j_pair < i_pair){
					int tmp = i_pair;
					i_pair = j_pair;
					j_pair = tmp;
				}

				//cout<<endl<<"sono dentro il while(1) con i_pair="<<i_pair<<" e j_pair="<<j_pair<<endl;

				// Add checked state
				state_pairs[i_pair].push_back(j_pair);

				int n = num_states_;
				int k = (n*(n-1)/2) - (n-i_pair)*((n-i_pair)-1)/2 + j_pair - i_pair - 1;


				if(distincts_table[k] == to_string(DFA_TF_STATE_N)){
					cerr << "ERROR! Required counterexample with equivalent states" << endl;
					throw witnessFromEquivalentDFA();
				}


				// Read the simbol which specifics the difference for the pair of states
				read_symbols = distincts_table[k];


				if(read_symbols == to_string(DFA_TF_STATE_X))
				{
					break;
				}


				wit.push_back(read_symbols);
				//cout<<"read_symbol = "<<read_symbols<<endl;
				//cout<<endl<<"da "<<i_pair<<" con "<<read_symbols;
				i_pair = get_ttable(i_pair,read_symbols);
				//cout<<" vado in "<<i_pair<<endl;
				//cout<<"da "<<j_pair<<" con "<<read_symbols;
				j_pair = get_ttable(j_pair,read_symbols);
				//cout<<" vado in "<<j_pair<<endl;
			}//end while(1)


			// Add generated symbol_s to characterization set
			if(wit.size() != 0)
			{
				// Prefix of characterizing symbol_s is the access symbol_s for analyzed states
				vector<symbol_> first_characterizing_symbol_s;
				vector<symbol_> second_characterizing_symbol_s;


				// Build characterzing symbol_s concatening access symbol_s with witness for analyzed states
				first_characterizing_symbol_s.insert( first_characterizing_symbol_s.end(), wit.begin(), wit.end() );
				second_characterizing_symbol_s.insert( second_characterizing_symbol_s.end(), wit.begin(), wit.end() );


				// Check if current sample is in the set yet
				if( std::find(characterization_set.begin(), characterization_set.end(), first_characterizing_symbol_s) == characterization_set.end() ){
					characterization_set.push_back(first_characterizing_symbol_s);
					//cout<<"sto aggiungendo al characterization_set la prima symbol_a: "<<first_characterizing_symbol_s<<endl;
				}
				if( std::find(characterization_set.begin(), characterization_set.end(), second_characterizing_symbol_s) == characterization_set.end() ){
					characterization_set.push_back(second_characterizing_symbol_s);
					//cout<<"sto aggiungendo al characterization_set la seconda symbol_a: "<<second_characterizing_symbol_s<<endl;
				}
			}

		}
	}

		//if (num_states_ == 1) //degenerate case, DFA with only a state. Return only the empty symbol_ in characterization set
		if(characterization_set.size()==0)
		{
		vector<symbol_> empty_symbol_;
		characterization_set.push_back(empty_symbol_);
	}

	return characterization_set;
}

vector<vector<symbol_> > 			ConcreteDfa::get_augmented_characterization_set(int sigma_exponent, vector<vector<symbol_> >& aug_characterization_set)const
{
	//cout<<endl<<"sigma exponent: "<<sigma_exponent<<endl;

	// Get simple characterization_set
	#ifdef DEBUG_DFA
	cout << "... START Simple char set creation..." << flush;
	#endif

	vector<vector<symbol_>> characterization_set = get_characterization_set();

	#ifdef DEBUG_DFA
	cout << "END Simple char set. Size: "<< characterization_set.size() << flush;
	#endif


	// Prefixes to be concatened
	vector<vector<symbol_>> prefixes;


	///INIT
	vector<symbol_> alph=get_alphabet();
	for(int j=0; j<get_dim_alphabet();++j){
		vector<symbol_> tmp;
		tmp.push_back(alph[j]);
		prefixes.push_back(tmp);
	}


		// If diff_of_states >0
		int prefix_length = sigma_exponent;
		prefix_length++;
		// If diff_of_states < 0
		if(sigma_exponent <= 0)
			prefix_length = 1;


		#ifdef DEBUG_DFA
		cout << "START creating prefixes to be concatenated..." << flush;
		#endif

    //cout<<endl<<"prefix_length= "<<prefix_length<<endl;
		// Create prefixes
		for(int i=0; i<prefix_length; ++i)
		{
			// Compute a new subset of symbol_s
			vector<vector<symbol_>> new_subset;
			for(symbol_ sym : alph)
			{
				for(auto &it : prefixes)
				{
					vector<symbol_> tmp_prefix = it;
					tmp_prefix.push_back(sym);
					new_subset.push_back(tmp_prefix);
					//if(new_subset.size() > LIMIT_OF_TESTSET_W_METHOD)
					//	throw wMethodTestSetTooBig();
				}
			}

			// It adds the new calculated sub set of symbol_s at "prefixes"
			for(auto &it : new_subset)
				prefixes.push_back(it);

		}

    vector<symbol_> empty_symbol_;
  	prefixes.push_back(empty_symbol_);

		#ifdef DEBUG_DFA
		cout << "END prefixes to be concatenated. Size: " << prefixes.size() << endl;
		cout << "START creating final aug char set..." << flush;
		#endif


		// It concatenates prefixes to the simple characterization_set
		for(auto &it1 : characterization_set)
		{
			//aug_characterization_set.push_back(it1);

			for(auto &it2 : prefixes){
				vector<symbol_> new_symbol_ = it2;

				new_symbol_.insert( new_symbol_.end(), it1.begin(), it1.end());

				aug_characterization_set.push_back(new_symbol_);
			}
			//cout << "Current size of aug char set: "<<aug_characterization_set.size() << flush;
			//if( aug_characterization_set.size() > LIMIT_OF_TESTSET_W_METHOD)
			//	throw wMethodTestSetTooBig();

		}


		#ifdef DEBUG_DFA
		cout << "END final aug char set. Size: " << aug_characterization_set.size() << flush;
		#endif

	return aug_characterization_set;
}

vector<vector<symbol_>>	ConcreteDfa::get_w_method_test_set(ConcreteDfa* target_dfa, bool sigma) const
{
	set<vector<symbol_>> w_method_test_set;

	#ifdef DEBUG_DFA
	cout << "START Cover set...";
	#endif

	vector<vector<symbol_>> cover_set = get_cover_set();
	vector<vector<symbol_>> cover_set_target_dfa=target_dfa->get_cover_set();

	#ifdef DEBUG_DFA
	cout << "END. Size: "<< cover_set.size() << endl;
	cout << "START aug char set ... " << flush;
	#endif

	int sigma_exponent=0;
	vector<vector<symbol_>> characterization_set;

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

	// Limit the number of symbol_s
	//if(cover_set.size() * aug_characterization_set.size() > 10000000000)
	//	throw wMethodTestSetTooBig();


	#ifdef DEBUG_DFA
	cout << "START Test set...";
	#endif

	////// Compute test set: concatenating cover set and aug. charac. set
	for(auto &it1 : cover_set)
	{
		// Add non-concatened symbol_ of cover set to final set
		//if(it1.size() != 0)
		//	w_method_test_set.insert(it1);


		// Add concatened symbol_s
		for(auto &it2 : characterization_set)
		{
			vector<symbol_> tmp_symbol_ = it1;

			// Concatenating symbol_s
			tmp_symbol_.insert( tmp_symbol_.end(), it2.begin(), it2.end() );

			// Add symbol_ to final set
			w_method_test_set.insert(tmp_symbol_);
		}
	}


	#ifdef DEBUG_DFA
	cout << "END. Size:" << w_method_test_set.size() << endl;
	#endif

	
	vector<vector<symbol_>> w_vec(w_method_test_set.begin(),w_method_test_set.end());

	return w_vec;
}

vector<long double> ConcreteDfa::get_w_method_statistics(vector<vector<symbol_>> test_set, ConcreteDfa* subject_dfa) const
{
  vector<long double> statistics;
  int tp=0;
  int fn=0;
  int tn=0;
  int fp=0;
  for(auto test : test_set){
    bool q1=this->membership_query(test);
    bool q2=subject_dfa->membership_query(test);
    if(q1){
      if(q2)  tp++;
      else  fn++;
    }
    else{
      if(q2)  fp++;
      else  tn++;
    }
  }

  long double precision=0;
  if((tp+fp)>0) precision=(long double)tp/((long double)tp+(long double)fp);
  long double recall=0;
  if((tp+fn)>0) recall=(long double)tp/((long double)tp+(long double)fn);
  long double f_measure=0;
  if((precision+recall)>0) f_measure=(2*precision*recall)/(precision+recall);
  long double specifity= (long double)tn/((long double)tn+(long double)fp);
  long double balanced_classification_rate=(recall+specifity)/2;

  statistics.push_back(tp);
  statistics.push_back(fn);
  statistics.push_back(tn);
  statistics.push_back(fp);
  statistics.push_back(precision);
  statistics.push_back(recall);
  statistics.push_back(f_measure);
  statistics.push_back(specifity);
  statistics.push_back(balanced_classification_rate);

  return statistics;
}

void ConcreteDfa::print_w_method(vector<long double> statistics)const{
	cout<<"============================"<<endl;
	cout<<"***** W-METHOD RESULTS *****"<<endl;
	cout<<"True Positives = "<<statistics[0]<<endl;
	cout<<"True Negatives = "<<statistics[2]<<endl;
	cout<<"False Positives = "<<statistics[3]<<endl;
	cout<<"False Negatives = "<<statistics[1]<<endl;
	cout<<"----------------------------"<<endl;
	cout<<"Precision = "<<statistics[4]<<endl;
	cout<<"Recall = "<<statistics[5]<<endl;
	cout<<"F-measure = "<<statistics[6]<<endl;
	cout<<"Specifity = "<<statistics[7]<<endl;
	cout<<"Balanced Classification Rate = "<<statistics[8]<<endl;
	cout<<"============================"<<endl;
}

void ConcreteDfa::print_dfa_similarity(ConcreteDfa* subject_dfa, bool sigma, bool isomorphism, bool color, double eps){
	vector<vector<symbol_>> test_set = get_w_method_test_set(subject_dfa,sigma);
	vector<long double> stats = get_w_method_statistics(test_set,subject_dfa);
	print_w_method(stats);
	vector<vector<double>> sim_matrix = neighbour_matching(subject_dfa,isomorphism,color,eps);
	print_structural_similarity(sim_matrix);
	long double similarity = (stats[6]+sim_matrix[num_states_][0])/2;
	cout<<"***** GLOBAL SIMILARITY *****"<<endl;
	cout<<"The global similarity score between the two dfas is: "<<similarity<<endl;
}

DfaSim ConcreteDfa::dfa_similarity(ConcreteDfa* subject_dfa, bool print, bool sigma, bool isomorphism, bool color, double eps){
	long double exec_time=0;
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	vector<vector<symbol_>> test_set = get_w_method_test_set(subject_dfa,sigma);
	vector<long double> stats = get_w_method_statistics(test_set,subject_dfa);
	vector<vector<double>> sim_matrix = neighbour_matching(subject_dfa,eps,color);

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	exec_time = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

	stats.push_back(exec_time);
	
	DfaSim sim = DfaSim(this,subject_dfa,stats,sim_matrix);

	if(print){
		sim.print_sim();
	}

	return sim;
	
}




/* Methods related to the "common dfa interface" */

vector<int> ConcreteDfa::sort_states(vector<vector<symbol_>>& sorted_phrases){
	vector<symbol_> sorted_alphabet = sort_alphabet();

	set<int> visited_states;
	vector<int> sorted_states;
	list<int> reached_states;
	list<vector<symbol_>> reached_phrases;

	reached_states.push_back(get_start_state());
	reached_phrases.push_back(vector<symbol_>());

	int remaining_states = get_num_states();
	while(remaining_states > 0){
		int state = reached_states.back();
		vector<symbol_> phrase = reached_phrases.back();

		reached_states.pop_back();
		reached_phrases.pop_back();

		if(visited_states.find(state) == visited_states.end()){
			--remaining_states;

			visited_states.insert(state);
			sorted_states.push_back(state);
			sorted_phrases.push_back(phrase);

			for(symbol_ s : sorted_alphabet){
				vector<symbol_> next_phrase = phrase;
				next_phrase.push_back(s);
				int next_state = get_ttable(state, s);
				if(visited_states.find(next_state) == visited_states.end()){
					reached_states.push_front(next_state);
					reached_phrases.push_front(next_phrase);
				}
			}
		}
	}

	return sorted_states;
}

/*vector<int> ConcreteDfa::sort_states(vector<vector<symbol_>>& sorted_phrases){

	vector<symbol_> sorted_alphabet = sort_alphabet();

	vector<symbol_> current_phrase;
	set<int> visited_states;
	vector<int> sorted_states;

	int remaining_states = get_num_states();

	do{
		int state = get_arrive_state(current_phrase);
		if(visited_states.find(state) == visited_states.end()){
			visited_states.insert(state);

			sorted_states.push_back(state);
			sorted_phrases.push_back(current_phrase);

			current_phrase.push_back(sorted_alphabet.front());
			--remaining_states;
		}
		else{
			current_phrase = get_next_phrase(current_phrase);
			if(current_phrase.empty())
				remaining_states = 0;
		}
	}
	while(remaining_states > 0);

	return sorted_states;
}*/

void ConcreteDfa::update_state_table(){

	state_table_.clear();
	vector<symbol_> sorted_alphabet = sort_alphabet();

	vector<vector<symbol_>> sorted_phrases;
	vector <int> sorted_states = sort_states(sorted_phrases);

	int i = 0;
	for(int state : sorted_states){
		state_table_.push_back(DfaState(is_accepting(state), sorted_phrases[i], i));
		++i;
	}

	i = 0;
	for(int state : sorted_states){
		for(symbol_ sym : sorted_alphabet){
			int next_state = get_ttable(state, sym);
			bool state_found = false;
			for(int j = 0; j < sorted_states.size() && !state_found; j++){
				if(sorted_states[j] == next_state){
					state_table_[i].set_transition(sym, &state_table_[j]);
					state_table_[j].set_incoming_transiction(std::make_pair(&state_table_[i], sym));
					state_found = true;
				}
			}
		}
		++i;
	}

	num_states_ = state_table_.size();
}

vector<DfaState>::iterator ConcreteDfa::begin(){
	return state_table_.begin();
}

vector<DfaState>::iterator ConcreteDfa::end(){
	return state_table_.end();
}

void ConcreteDfa::print_state_table(){
	for(auto state : *this){
		state.print();
	}
}

/*
 * Construct a random degree-2 digraph on (5/4)*n nodes, extract
 * the subgraph reachable from the randomly chosen root node, and
 * label the graph's states by flipping a fair coin.
*/
void ConcreteDfa::random_dfa_abbadingo(int n, int seed, int n_symbols, string file_path){

	srand (time(NULL));

	int nodes_number = ceil(n * 5 / 4);

	vector<symbol_> simboli;
	simboli.push_back("a");
	simboli.push_back("b");
	simboli.push_back("c");
	simboli.push_back("d");
	simboli.push_back("e");
	simboli.push_back("f");
	simboli.push_back("g");
	simboli.push_back("h");
	simboli.push_back("i");
	simboli.push_back("j");
	simboli.push_back("k");
	simboli.push_back("l");
	simboli.push_back("m");
	simboli.push_back("n");
	simboli.push_back("o");
	simboli.push_back("p");
	simboli.push_back("q");
	simboli.push_back("r");
	simboli.push_back("s");
	simboli.push_back("t");
	simboli.push_back("u");
	simboli.push_back("v");
	simboli.push_back("w");
	simboli.push_back("x");
	simboli.push_back("y");
	simboli.push_back("z");

	vector<symbol_> alphabet;
	for(int i=0; i<n_symbols; i++){
		alphabet.push_back(simboli[i]);
	}

	int alphabet_size = alphabet.size();

	int start_node = (rand() * seed) % (nodes_number-1);
	//int start_node = 1;

	default_random_engine e;
	e.seed((rand() * 383 * seed));
	uniform_int_distribution<unsigned> accettante(0, 1);
	uniform_int_distribution<unsigned> nodo_arrivo(0, (nodes_number-1));

	vector<map<symbol_,int>> ttable = vector<map<symbol_,int>>();
	vector<int> accepting_states = vector<int>();
	
	for(int i = 0; i < nodes_number; ++i){
		map<symbol_,int> tmp=map<symbol_,int>();
		for(symbol_ sym : alphabet){
			tmp[sym] = nodo_arrivo(e);
		}
		ttable.push_back(tmp);
		if(accettante(e))
			accepting_states.push_back(1);
		else
			accepting_states.push_back(0);
	}

	ConcreteDfa temp(nodes_number, alphabet, start_node, ttable, accepting_states);

	temp.strict_ = false;

	temp.update_state_table();

	temp.remove_unreachable();

	//temp.print_dfa_ttable("After remove unreachable");

	while(nodes_number - temp.get_num_states() > 0){
		int tmp_states = temp.get_num_states();
		
		for(DfaState state : temp){
			// possiamo cambiare una transizione da uno stato che ne ha in ingresso almeno 2
			// e ridirigerla su un nuovo stato
			vector<pair<DfaState*, symbol_> > removable = state.get_removable_incoming_transictions();

			int rem_size = removable.size();

			//cout <<"Stato: " <<state.get_index()<< " dim removable = " << rem_size <<endl;

			if(rem_size > 0){

				uniform_int_distribution<unsigned> transizione_rand(0, rem_size-1);

				int cas = transizione_rand(e);
				
				int index = removable[cas].first->get_index();

				symbol_ symbol = removable[cas].second;
				
				temp.ttable_[index][symbol] = tmp_states;

				map<symbol_, int> new_trans=map<symbol_,int>();
				
				for(symbol_ sym : alphabet){
					uniform_int_distribution<unsigned> new_tr(0, tmp_states);
					new_trans[sym] = new_tr(e);
				}

				int accept = accettante(e);

				if(accept)
					temp.accepting_states_.push_back(tmp_states);
				
				temp.ttable_.push_back(new_trans);
				temp.num_states_ = tmp_states + 1;

				temp.update_state_table();

				temp.remove_unreachable();

				
				break;

			}
		}

	}

	temp.print_dfa_in_text_file(file_path.c_str());

}

void ConcreteDfa::remove_unreachable(){
	start_state_ = 0;
	num_states_ = state_table_.size();
	ttable_.clear();
	accepting_states_.clear();
	for(DfaState state : *this){
		if(state.is_accepting())
			accepting_states_.push_back(state.get_index());
		map<symbol_, int> tmp;
		for(symbol_ sym : alphabet_){
			tmp[sym] = state.next(sym)->get_index();
		}
		ttable_.push_back(tmp);
	}
}

pair<double, unsigned int> ConcreteDfa::struct_sim(ConcreteDfa* subject_dfa, bool isomorphism, bool color, double eps) {
	vector<vector<double> > sim = neighbour_matching(subject_dfa, isomorphism, color, eps);
	return std::make_pair(sim[num_states_][0], sim[num_states_][1]);
}

