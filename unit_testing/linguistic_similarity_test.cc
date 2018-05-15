#include <iostream>
#include <numeric>
#include "gtest/gtest.h"

#include "testDfa.h"

#define DFA_TF_STATE_X numeric_limits<short int>::max() -1  	// Distinct state (accepting vs rejecting)



class LinguisticSimilarityTest : public ::testing::Test {
protected:

	LinguisticSimilarityTest() {
		// You can do set-up work for each test here.
	}

	virtual ~LinguisticSimilarityTest() {
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
	vector<string> alph={"a","b","c"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	TestDfa* reference = new TestDfa(5,alph);
	TestDfa* subject = new TestDfa(5,alph);

};


TEST_F(LinguisticSimilarityTest, coverSet){
  bool flag=true;
	vector<vector<string>> cover_set_ref={{""},{"a"},{"b"},{"c"},{"a","a"}};
	vector<vector<string>> cover_set=reference->get_cover_set();
  if(cover_set.size()!=cover_set_ref.size())
    flag=false;
  if(flag)
    for(int i =0; i<cover_set.size();++i)
    {
      if(!(equal(cover_set[i].begin(), cover_set[i].end(), cover_set_ref[i].begin()))){
        flag=false;
        break;
      }
    }

	EXPECT_EQ(1,flag);
}



TEST_F(LinguisticSimilarityTest, characterizationSet){
	vector<vector<string>> characterization_set_ref={{"a"},{"b"},{"a","a"}};
  bool flag=true;
	vector<vector<string>> characterization_set=reference->get_characterization_set();
	if(characterization_set.size()!=characterization_set_ref.size()){
    cout<<"WRONG SIZE"<<endl;
    flag=false;
  }
  if(flag)
    for(int i =0; i<characterization_set.size();++i)
    {
      if(!(equal(characterization_set[i].begin(), characterization_set[i].end(), characterization_set_ref[i].begin()))){
        flag=false;
        break;
      }
    }

	EXPECT_EQ(1,flag);
}

TEST_F(LinguisticSimilarityTest, reflectiveTestSet) {
	vector<vector<string>> test_set=reference->get_w_method_test_set(subject);
  vector<long double> stat=reference->get_w_method_statistics(test_set,subject);
	EXPECT_EQ(1,stat[6]); //stat[6] is f_measure
}

TEST_F(LinguisticSimilarityTest, tableFilling){
  //DFA_TF_STATE_X stands for distinct states
	vector<string> distincts_table_ref={"a", "b", to_string(DFA_TF_STATE_X), "a", "a", to_string(DFA_TF_STATE_X), "a", to_string(DFA_TF_STATE_X), "b", to_string(DFA_TF_STATE_X)};
  vector<string> distincts_table=reference->table_filling();
	EXPECT_EQ(1,equal(distincts_table_ref.begin(),distincts_table_ref.end(),distincts_table.begin()));
}


TEST_F(LinguisticSimilarityTest, accessStrings){
	vector<string> access_strings_ref={"a","b","aa","c"};
	map<int, vector<string> > access_strings=reference->get_access_strings();
	vector<string> access_strings1;
  for(auto it : access_strings)
  {
    vector<string> strvectest;
    for (string sytest:it.second)
      strvectest.push_back(sytest);
		string str1;
    str1=std::accumulate(strvectest.begin(),strvectest.end(),str1);
    access_strings1.push_back(str1);
	}
	EXPECT_EQ(1,equal(access_strings_ref.begin(),access_strings_ref.end(),access_strings1.begin()));
}

