#include <iostream>
#include <numeric>
#include "gtest/gtest.h"

#include "testBlueFringe.h"

class BlueFringeTest : public ::testing::Test {
protected:

	BlueFringeTest() {
		// You can do set-up work for each test here.
	}

	virtual ~BlueFringeTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Project1.
    const char* path="./data/read_samples_test.txt";
	TestBlueFringe test(path);

};


TEST_F(BlueFringeTest, readSamples){
    
    vector<SYMBOL>* positive = NULL;
    vector<SYMBOL>* negative = NULL;
    int dim_positive = 0; 		
	int dim_negative = 0; 		
	int *wp, *wn;

    test.read_samples(positive, &dim_positive, negative, &dim_negative, wp, wn);

    vector<SYMBOL>* ref_positive=NULL;
    ref_positive[0].push_back(0);
    ref_positive[1].push_back(0);
    ref_positive[1].push_back(0);
    ref_positive[2].push_back(0);
    ref_positive[2].push_back(0);
    ref_positive[2].push_back(0);
    ref_positive[3].push_back(0);
    ref_positive[3].push_back(0);
    ref_positive[3].push_back(0);
    ref_positive[3].push_back(0);

    vector<SYMBOL>* ref_negative=NULL;
    ref_negative[0].push_back(1);
    ref_negative[1].push_back(1);
    ref_negative[1].push_back(1);
    ref_negative[2].push_back(1);
    ref_negative[2].push_back(1);
    ref_negative[2].push_back(0);
    ref_negative[3].push_back(1);
    ref_negative[3].push_back(0);
    ref_negative[4].push_back(1);
    ref_negative[4].push_back(1);
    ref_negative[4].push_back(1);
    ref_negative[4].push_back(0);
    ref_negative[5].push_back(1);
    ref_negative[5].push_back(1);
    ref_negative[5].push_back(1);

    bool compare = true;

    for(int i = 0; i<dim_positive; i++){
		if(positive[i]!=ref_positive[i])
            compare=false;
	}

    for(int j = 0; j<dim_negative; j++){
		if(negative[j]!=ref_negative[j])
            compare=false;
	}

	EXPECT_EQ(1,compare);

}