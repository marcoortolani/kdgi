#include <iostream>
#include <numeric>
#include <vector>
#include <string>
using namespace std;
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
    const char* path="../unit_testing/data/read_samples_test.txt";
	TestBlueFringe* test = new TestBlueFringe(path);

};


TEST_F(BlueFringeTest, readSamples){
    
    vector<string>* positive = NULL;
    vector<string>* negative = NULL;
    int dim_positive = 0; 		
	int dim_negative = 0; 		
	int *wp = NULL;
    int *wn = NULL;

    test->read_samples(positive, &dim_positive, negative, &dim_negative, wp, wn);
    
    vector<string> ref_positive={"a","aa","aaa","aaaa"};

    vector<string> ref_negative={"b","bb","bba","ba","bbba","bbb"};

    bool compare = true;

    for(int i = 0; i<dim_positive; i++){
        vector<string> tmp = positive[i];
        string a = std::accumulate(tmp.begin(), tmp.end(), string(""));
		if(a!=ref_positive[i])
            compare=false;
	}

    for(int j = 0; j<dim_negative; j++){
        vector<string> tmp = negative[j];
        string a = std::accumulate(tmp.begin(), tmp.end(), string(""));
		if(a!=ref_negative[j])
            compare=false;
	}
    

	EXPECT_EQ(1,compare);

}