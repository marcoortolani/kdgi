#include <iostream>
#include <numeric>
#include <vector>
#include <string>
using namespace std;
#include "gtest/gtest.h"

#include "angluindfa.h"
#include "angluinlearner.h"
#include "sillyoracle.h"

#include "testDfa.h"

class BaseAngTest : public ::testing::Test {
protected:

	BaseAngTest() {
		// You can do set-up work for each test here.
	}

	virtual ~BaseAngTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
		reference->set_ttable_from_sequence(sequence);
        reference->update_state_table();
        
        ang = l.learn();
        
        subject->set_ttable_from_sequence(sequence2);
        subject->update_state_table();
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
		delete ang;
	}

	// Objects declared here can be used by all tests in the test case for Project1.
	vector<string> alph={"0","1","2"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	TestDfa* reference = new TestDfa(5,alph);
	
	ConcreteDfa* concrete_reference = reference;
	SillyOracle o = SillyOracle(concrete_reference);
	AngluinLearner l = AngluinLearner(&o, alph);
	AngluinDfa* ang;
	
	vector<int> sequence2 = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 3, 1, 4, 4, 4, 0};
	TestDfa* subject = new TestDfa(5,alph);
};


TEST_F(BaseAngTest, getDimAlphabet){
	EXPECT_EQ(3, ang->get_dim_alphabet());
}

TEST_F(BaseAngTest, isIdentical){
	vector<symbol_> phrase;
	EXPECT_EQ(1, ang->is_identical(concrete_reference, phrase));
}

TEST_F(BaseAngTest, structuralSimilarity){
	bool t = true;
	vector<vector<double>> v;
	
	v = ang->neighbour_matching(ang);
	t = t && static_cast<bool>((v.back()).front());
	
	v = ang->neighbour_matching(concrete_reference);
	t = t && static_cast<bool>((v.back()).front());
	
	v = concrete_reference->neighbour_matching(ang);
	t = t && static_cast<bool>((v.back()).front());
	
	v = concrete_reference->neighbour_matching(concrete_reference);
	t = t && static_cast<bool>((v.back()).front());
	
	EXPECT_EQ(1, t);
}

TEST_F(BaseAngTest, equivalence_query){
	bool t = true;
	
	vector <symbol_> counterexample;
	t = t && ang->equivalence_query(reference, counterexample);
	t = t && (*reference).Dfa::equivalence_query(ang, counterexample);
	
	bool equivalent = ang->equivalence_query(subject, counterexample);
	t = t && ! equivalent;
	
	equivalent = equivalent == ((*ang)[counterexample]->is_accepting() == (*subject)[counterexample]->is_accepting());
	t = t && equivalent;
	
	EXPECT_EQ(1, t);
}
