//******** Thin-wrapper to do unit-testing of abstract classes ********
#ifndef TEST_BLUEFRINGE_H_
#define TEST_BLUEFRINGE_H_
#include "bluefringe.h"

class TestBlueFringe : public BlueFringe{

  friend class BlueFringeTest;

  FRIEND_TEST(BlueFringeTest, readSamples);

  //concretize the abstract class giving empty body to pure virtual functions
  Dfa* run(string path, double exec_time) {};
  int merge_heuristic_score(RedBlueDfa* dfa1, vector<string>* positive, int dim_positive, vector<string>* negative, int dim_negative, int* wp , int* wn){};
	double merge_heuristic_score(double error_rate_before, double error_rate_after, int dim_strings, double alpha, int earn_states){};
	double merge_heuristic_score(double error_rate_before, double error_rate_after, int dim_strings, double alpha, double earn_states){};

public:
  using BlueFringe::BlueFringe;
};

#endif
