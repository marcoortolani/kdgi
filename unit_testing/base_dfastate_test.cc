#include <iostream>
#include <numeric>
#include <vector>
#include <string>
using namespace std;
#include "gtest/gtest.h"

#include "testDfa.h"

class BaseDfaStateTest : public ::testing::Test {
protected:

	BaseDfaStateTest() {
		// You can do set-up work for each test here.
	}

	virtual ~BaseDfaStateTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
		reference->set_ttable_from_sequence(sequence);
        reference->update_state_table();
        
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Project1.
	vector<string> alph={"0","1","2"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	TestDfa* reference = new TestDfa(5,alph);
	
};

TEST_F(BaseDfaStateTest, get_index){
	
	symbol_ zero = "0";
	symbol_ uno = "1";
	symbol_ due = "2";
	vector<vector<symbol_>> phrases = {vector<symbol_>(), vector<symbol_>{zero}, vector<symbol_>{uno}, vector<symbol_>{due}, vector<symbol_>{zero,zero}};
	bool t = true;
	
	int i = 0;
	for(DfaState s : *reference){
		t = t && s.get_index() == i && s.get_charact_phrase() == phrases[i];
		++i;
	}
	
	EXPECT_EQ(1, t);
}

TEST_F(BaseDfaStateTest, get_outcoming_states){
	vector<symbol_> acc = {"0", "0"};
	DfaState* s = ((*reference)[acc]);
	DfaState* s0 = s->next("0");
	vector<DfaState*> v = s->get_outcoming_states();
	EXPECT_EQ(1, v.size() == 1 && v[0] == s0);
}

TEST_F(BaseDfaStateTest, get_incoming_states){
	vector<symbol_> acc = {"0", "0", "0"};
	DfaState* s = ((*reference)[acc]);
	bool t = true;
	
	set<vector<symbol_>> all_states;
	vector<DfaState*> incoming_states = s->get_incoming_states();
	
	for(DfaState* s1 : incoming_states){
		all_states.insert(s1->get_charact_phrase());
	}
	
	for(DfaState s1 : *reference){
		vector<symbol_> a = s1.get_charact_phrase();
		if(all_states.find(a) == all_states.end()){
			cout << a;
			t = false;
		}
	}
	
	EXPECT_EQ(1, t);
}
