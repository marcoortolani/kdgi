/*
 * angluinlearner.h
 *
 *  Created on: 17/mag/2018
 *      Author: marco
 */

#ifndef ANGLUINLEARNER_H_
#define ANGLUINLEARNER_H_

#include "learner.h"
#include "angluindfa.h"

class AngluinLearner: public Learner {
private:
	AngluinDfa* ang_dfa;
	void make_mem_queries(vector<vector<string>> prefixes, vector<vector<string>> suffixes, vector<vector<bool>>* mem_query_res);
public:
	AngluinLearner(Oracle* o, vector<string> alphabet);
	virtual Dfa* learn();
};

#endif /* ANGLUINLEARNER_H_ */
