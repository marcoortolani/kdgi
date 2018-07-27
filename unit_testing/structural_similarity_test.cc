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
		reference->set_ttable_from_sequence(sequence);
		subject->set_ttable_from_sequence(sequence);
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Project1.
	const vector<string> alph = {"a","b","c"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	//const vector<int> sequence2 = {1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 2, 0, 0, 0, 3, 1, 4, 4, 4, 0};
	TestDfa* reference = new TestDfa(5,alph);
	TestDfa* subject = new TestDfa(5,alph);

};


TEST_F(StructuralSimilarityTest, reflectiveTestSet) {
  	vector<vector<double>> similarity_matrix = reference->neighbour_matching(subject);
	//similarity_matrix[reference->get_num_states()][0] contains the final structural sim score
	EXPECT_EQ(1,similarity_matrix[reference->get_num_states()][0]);
	//EXPECT_EQ(1,1);
}
