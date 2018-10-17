#ifndef ANGLUINLEARNER_H_
#define ANGLUINLEARNER_H_

#include "angluindfa.h"
#include "learner.h"

template <class Oracle>
class AngluinLearner : public Learner<vector<DfaState>*, Oracle>{
private:

	/**
	 * It simply asks the oracle whether each phrase is a member
	 * of the language and stores the result in the same position.
	 * @param phrases_to_verify 	the list of phrases which we need to ask for membership.
	 * @param mem_query_res 		the results of the queries will be stored here.
	 */
	void make_mem_queries(map<list<string>, bool>* mem_query_res);

public:

	/**
	 * Constructor. Probably the alphabet is not needed any more.
	 * @param o		the oracle which will respond to all the membership and equivalence queries.
	 */
	AngluinLearner(Oracle* o, vector <symbol_> ab);

	/**
	 * It implements the high level steps of the Angluin's algorithm
	 * (all the low level operation are done by the dfa itself).
	 */
	AngluinDfa* learn();
};

#include "angluinlearner.tcc"

#endif /* ANGLUINLEARNER_H_ */
