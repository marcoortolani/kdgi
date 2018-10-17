#ifndef TTTLEARNER_H_
#define TTTLEARNER_H_

#include "tttdfa.h"
#include "learner.h"

template<class Oracle>
class TTTLearner : public Learner<list<DfaState>*, Oracle>{
private:
	/*Oracle* oracle;
	vector <symbol_> alphabet;*/

	bool observation_pack_;

	void close_transition(TTTDfa* t, tuple<vector<string,allocator<string>>,string,bool> pair);
	void close_transitions(TTTDfa* t, bool hard_sift);
	void try_finalization(TTTDfa* t);

	TTTDfa* OPACKlearn();
	TTTDfa* TTTlearn();

public:
	void set_opack(bool opack);

	TTTLearner(Oracle* o, vector<string> alphabet, bool opack = false);

	TTTDfa* learn();
};

#include "tttlearner.tcc"

#endif /* TTTLEARNER_H_ */
