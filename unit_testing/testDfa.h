//******** Thin-wrapper to do unit-testing of protected methods ********
#include "concretedfa.h"

class TestDfa : public ConcreteDfa{
  friend class BaseDfaTest;
  friend class LinguisticSimilarityTest;
  friend class StructuralSimilarityTest;
  friend class BaseAngTest;
  friend class BaseDfaStateTest;

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
  using ConcreteDfa::ConcreteDfa;
};
