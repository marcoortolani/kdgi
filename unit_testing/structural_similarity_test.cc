#include <iostream>
#include <numeric>
#include "gtest/gtest.h"

#include "testDfa.h"


class StructuralSimilarityTest : public ::testing::Test {
protected:

	StructuralSimilarityTest() {
		// You can do set-up work for each test here.
	}

	virtual ~StructuralSimilarityTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
		reference_Tomita15->set_ttable_from_sequence(sequence);
		subject_Tomita14->set_ttable_from_sequence(sequence2);
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Project1.
	const string alph[3] = {"a","b","c"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	const string alph2[2] = {"a","b"};
	const vector<int> sequence2 = {1, 2, 0, 0, 3, 0, 2, 3, 1, 3, 3, 0};
	TestDfa* reference_Tomita15 = new TestDfa(5,3,alph);
	TestDfa* subject_Tomita14 = new TestDfa(4,2,alph2);

};


TEST_F(StructuralSimilarityTest, succ_ab){
	set<vector<SYMBOL>> matching_transitions_ref=reference_Tomita15->succ_ab(0,0,subject_Tomita14);

	set<vector<SYMBOL>> matching_transitions;

	EXPECT_EQ(1,1);
}
