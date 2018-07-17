#ifndef ANGLUINLEARNER_H_
#define ANGLUINLEARNER_H_

#include "angluindfa.h"
#include "learner.h"

class AngluinLearner: public Learner<vector<DfaState>*>{
private:
	/**
	 * Dfa structure used by Angluin algorithm.
	 * It contains:
	 * a prefix-closed list of sequence of symbol S,
	 * a list of sequence of symbol Sa,
	 * a suffix-closed list of sequence of symbols E,
	 * methods for verifying it's closure and consistency.
	 */
	AngluinDfa* ang_dfa;

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
	AngluinLearner(Oracle* o, vector<string> alphabet);

	/**
	 * It initializes ang_dfa and implements the high level steps of the Angluin's algorithm
	 * (all the low level operation are done by the ang_dfa).
	 */
	AngluinDfa* learn();
};

#endif /* ANGLUINLEARNER_H_ */
