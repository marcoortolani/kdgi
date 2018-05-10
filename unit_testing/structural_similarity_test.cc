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
		subject_Tomita15->set_ttable_from_sequence(sequence);
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Project1.
	const string alph[3] = {"a","b","c"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	TestDfa* reference_Tomita15 = new TestDfa(5,3,alph);
	TestDfa* subject_Tomita15 = new TestDfa(5,3,alph);

};


TEST_F(StructuralSimilarityTest, reflectiveTestSet) {

  double** similarity_matrix = reference_Tomita15->neighbour_matching_structural_similarity(subject_Tomita15,0.0001);

	EXPECT_EQ(1,similarity_matrix[reference_Tomita15->get_num_states()][0]);
}
