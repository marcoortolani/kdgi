#include <iostream>
#include <numeric>
#include "gtest/gtest.h"

#include "testDfa.h"


class BaseDfaTest : public ::testing::Test {
protected:

	BaseDfaTest() {
		// You can do set-up work for each test here.
	}

	virtual ~BaseDfaTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
		reference_Tomita15->set_ttable_from_sequence(sequence);
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Project1.
	const string alph[3] = {"a","b","c"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	TestDfa* reference_Tomita15 = new TestDfa(5,3,alph);

};


TEST_F(BaseDfaTest, getDimAlphabet){
	EXPECT_EQ(3,reference_Tomita15->get_dim_alphabet());
}


TEST_F(BaseDfaTest, getAlphabet){
    const string alph[3]={"a","b","c"};
    const string* alph_test=reference_Tomita15->get_alphabet();
    bool equal=true;
    int i;
    for(i=0;i<3;i++){
        if(alph[i]!=alph_test[i])
            equal=false;
    }
    EXPECT_EQ(1,equal);
}


TEST_F(BaseDfaTest, getNumStates){
    EXPECT_EQ(5,reference_Tomita15->get_num_states());
}


TEST_F(BaseDfaTest, getStartState){
    EXPECT_EQ(0,reference_Tomita15->get_start_state());
}


TEST_F(BaseDfaTest, readDfaFile){
    Dfa* test=nullptr; 
    test=new Dfa(test->read_dfa_file("../unit_testing/data/tomita15.txt"));
    int** ttable_test=test->get_ttable();
    int** ttable_ref=reference_Tomita15->get_ttable();
    bool equal=true;
    int i,j;
    for(i=0;i<5;i++){
        for(j=0;j<4;j++)
            if(ttable_test[i][j]!=ttable_ref[i][j]){
                equal=false;
                break;
            }
        if(!equal)
            break;
    }
    EXPECT_EQ(1,equal);
}