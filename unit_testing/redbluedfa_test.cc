#include <iostream>
#include <numeric>
#include <vector>
#include <string>
using namespace std;
#include "gtest/gtest.h"

#include "testRedBlueDfa.h"

class RedBlueDfaTest : public ::testing::Test {
protected:

	RedBlueDfaTest() {
		// You can do set-up work for each test here.
	}

	virtual ~RedBlueDfaTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
        reference->set_ttable_from_sequence(sequence);
        subject->set_ttable_from_sequence(sequence);
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Project1.
    vector<string> alph={"0","1","2"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	TestRedBlueDfa* reference = new TestRedBlueDfa(5,alph);
    TestRedBlueDfa* subject = new TestRedBlueDfa(5,alph);

};


TEST_F(RedBlueDfaTest, getDimAlphabet){
    EXPECT_EQ(3,reference->get_dim_alphabet());
}

TEST_F(RedBlueDfaTest, getTransition){
    EXPECT_EQ(3,reference->get_ttable(1,"0"));
}

TEST_F(RedBlueDfaTest, setAcceptingState){
    reference->set_accepting_states_entry(1,1);
    EXPECT_EQ(1,reference->get_accepting_states()[1]);
    reference->set_rejecting_state(1);
    EXPECT_EQ(0,reference->get_accepting_states()[1]);
}

TEST_F(RedBlueDfaTest, getAcceptingStates){
    vector<int> acc={0,0,0,1,0};
    vector<int> acc_ref=reference->get_accepting_states();
    bool flag=true;
    for(int i=0; i<reference->get_num_states();++i){
        //cout<<acc_ref[i];
        if(acc[i]!=acc_ref[i]){
            flag=false;
            break;
        }
        //cout<<endl;
    }
    EXPECT_EQ(1,flag);
}

TEST_F(RedBlueDfaTest, copyConstructor){
    TestRedBlueDfa* test = new TestRedBlueDfa(*reference);
    EXPECT_EQ(1,*test==*reference);
}