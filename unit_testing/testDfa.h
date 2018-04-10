//******** Thin-wrapper to do unit-testing of protected methods ********
#include "dfa.h"

class TestDfa : public Dfa{

  friend class LinguisticSimilarityTest;
  friend class StructuralSimilarityTest;

  FRIEND_TEST(LinguisticSimilarityTest, coverSet);
  FRIEND_TEST(LinguisticSimilarityTest, characterizationSet);
  FRIEND_TEST(LinguisticSimilarityTest, accessStrings);
  FRIEND_TEST(LinguisticSimilarityTest, tableFilling);

  FRIEND_TEST(StructuralSimilarityTest, succ_ab);

public:
  using Dfa::Dfa;
};
