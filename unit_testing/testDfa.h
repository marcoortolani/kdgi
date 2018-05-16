//******** Thin-wrapper to do unit-testing of protected methods ********
#include "dfa.h"

class TestDfa : public Dfa{
  friend class BaseDfaTest;
  friend class LinguisticSimilarityTest;
  friend class StructuralSimilarityTest;

  FRIEND_TEST(BaseDfaTest, equivalenceQuery);
  FRIEND_TEST(BaseDfaTest, setAcceptingState);
  FRIEND_TEST(BaseDfaTest, operatorEqual);
  FRIEND_TEST(BaseDfaTest, operatorEqualEqual);
  FRIEND_TEST(BaseDfaTest, unionDfa);

  FRIEND_TEST(LinguisticSimilarityTest, coverSet);
  FRIEND_TEST(LinguisticSimilarityTest, characterizationSet);
  FRIEND_TEST(LinguisticSimilarityTest, accessStrings);
  FRIEND_TEST(LinguisticSimilarityTest, tableFilling);
/*
  TestDfa* unionDFA(TestDfa* dfa_hp)
{
	int count_state = dfa_hp->num_states_ + num_states_;

	// Instance of union dfa
	Dfa* union_dfa = new Dfa(count_state, alphabet_, start_state_);

	// Configuration of Union DFA
	// Smaller indexes are given to target dfa, while others to hypothesis
	for(int j=0; j<num_states_; ++j)									// Target automaton
		for(string sym : alphabet_)
			union_dfa->set_ttable_entry(j,sym,get_ttable(j,sym));

	for(int j=0; j<dfa_hp->num_states_; ++j)						// Hypothesis automaton
		for(string sym_hp : alphabet_)					// In union dfa, start state of HP dfa is recorded in "num_state" index of target dfa
			union_dfa->set_ttable_entry(num_states_+j,sym_hp, (dfa_hp->get_ttable(j,sym_hp) + num_states_));

	for(int i=0; i<num_states_;++i)
		if(is_accepting(i))
			union_dfa->accepting_states_[i]=1;
	for(int j=0; j<dfa_hp->num_states_;++j)
		if(dfa_hp->is_accepting(j))
			union_dfa->accepting_states_[j+num_states_]=1;

	return union_dfa;
}*/

public:
  using Dfa::Dfa;
};
