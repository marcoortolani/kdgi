#include <iostream>
#include <numeric>
#include "gtest/gtest.h"

#include "testDfa.h"


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


TEST_F(LinguisticSimilarityTest, coverSet){

	vector<string> cover_set_ref={"","a","b","c","aa"};
	vector<vector<SYMBOL > > cover_set_mapped=reference_Tomita15->get_cover_set();
	vector<string> cover_set1;
	for(auto phrase1 : cover_set_mapped)
  {
    vector<string> strvec1;
    for(SYMBOL sy1:phrase1)
    {
      string tmp1=reference_Tomita15->get_letter_from_mapped_alphabet(sy1);
      strvec1.push_back(tmp1);
    }
    string str1;
    str1=std::accumulate(strvec1.begin(),strvec1.end(),str1);
    cover_set1.push_back(str1);
  }

	EXPECT_EQ(1,equal(cover_set_ref.begin(),cover_set_ref.end(),cover_set1.begin()));
}


TEST_F(LinguisticSimilarityTest, characterizationSet){
	vector<string> characterization_set_ref={"a","b","aa"};

	vector<vector<SYMBOL > > characterization_set_mapped=reference_Tomita15->get_characterization_set();
	vector<string> characterization_set1;
	for(auto phrase1 : characterization_set_mapped)
  {
    vector<string> strvec1;
    for(SYMBOL sy1:phrase1)
    {
      string tmp1=reference_Tomita15->get_letter_from_mapped_alphabet(sy1);
      strvec1.push_back(tmp1);
    }
    string str1;
    str1=std::accumulate(strvec1.begin(),strvec1.end(),str1);
    characterization_set1.push_back(str1);
  }

	EXPECT_EQ(1,equal(characterization_set_ref.begin(),characterization_set_ref.end(),characterization_set1.begin()));
}


TEST_F(LinguisticSimilarityTest, reflectiveTestSet) {
	vector<string> test_set=reference_Tomita15->get_w_method_test_set(subject_Tomita15,true);

	int tp=0;
  int fn=0;
  int tn=0;
  int fp=0;
  for(auto test : test_set){
    bool q1=reference_Tomita15->membership_query(test);
    bool q2=subject_Tomita15->membership_query(test);
    if(q1){
      if(q2)  tp++;
      else  fn++;
    }
    else{
      if(q2)  fp++;
      else  tn++;
    }
  }

  long double precision= (long double)tp/((long double)tp+(long double)fp);
  long double recall= (long double)tp/((long double)tp+(long double)fn);
  long double f_measure= (2*precision*recall)/(precision+recall);

	EXPECT_EQ(1,f_measure);
}


TEST_F(LinguisticSimilarityTest, tableFilling){
	vector<string> distincts_table_ref={"a", "b", "distinct", "a", "a", "distinct", "a", "distinct", "b", "distinct"};

  SYMBOL* distincts_table=reference_Tomita15->table_filling();
  int n=reference_Tomita15->get_num_states();
	vector<string> distincts_table1;
  for(int i=0;i<(n*(n-1)/2);++i){
		SYMBOL tmp=distincts_table[i];
		if (tmp<reference_Tomita15->get_dim_alphabet()){
			string entry = reference_Tomita15->get_letter_from_mapped_alphabet(distincts_table[i]);
			distincts_table1.push_back(entry);
		}
    else	distincts_table1.push_back("distinct");
	}

	EXPECT_EQ(1,equal(distincts_table_ref.begin(),distincts_table_ref.end(),distincts_table1.begin()));
}


TEST_F(LinguisticSimilarityTest, accessStrings){
	vector<string> access_strings_ref={"a","b","aa","c"};

	map<int, vector<SYMBOL> > access_strings=reference_Tomita15->get_access_strings_mapped_alphabet();
	vector<string> access_strings1;
  for(auto it : access_strings)
  {
    vector<string> strvectest;
    for (SYMBOL sytest:it.second)
    {
      string tmptest=reference_Tomita15->get_letter_from_mapped_alphabet(sytest);
      strvectest.push_back(tmptest);
    }
		string str1;
    str1=std::accumulate(strvectest.begin(),strvectest.end(),str1);
    access_strings1.push_back(str1);
	}

	EXPECT_EQ(1,equal(access_strings_ref.begin(),access_strings_ref.end(),access_strings1.begin()));
}
