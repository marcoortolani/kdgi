//******** Thin-wrapper to do unit-testing of protected methods ********
#ifndef TEST_DFA_H_
#define TEST_DFA_H_
#include "dfa.h"

class TestDfa : public Dfa{
  friend class BaseDfaTest;
  friend class LinguisticSimilarityTest;
  friend class StructuralSimilarityTest;

  FRIEND_TEST(BaseDfaTest, equivalenceQuerySelfTest);
  FRIEND_TEST(BaseDfaTest, setAcceptingState);
  FRIEND_TEST(BaseDfaTest, operatorEqual);
  FRIEND_TEST(BaseDfaTest, operatorEqualEqual);
  FRIEND_TEST(BaseDfaTest, unionDfa);

  FRIEND_TEST(LinguisticSimilarityTest, coverSet);
  FRIEND_TEST(LinguisticSimilarityTest, characterizationSet);
  FRIEND_TEST(LinguisticSimilarityTest, accessStrings);
  FRIEND_TEST(LinguisticSimilarityTest, tableFilling);

public:
  using Dfa::Dfa;
};

#endif
