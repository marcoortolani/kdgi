#include "dfa.h"

class TestDfa : public Dfa{
  friend class Tomita15Test;
  FRIEND_TEST(Tomita15Test, coverSet);
  FRIEND_TEST(Tomita15Test, characterizationSet);
  FRIEND_TEST(Tomita15Test, accessStrings);
  FRIEND_TEST(Tomita15Test, tableFilling);

public:
  using Dfa::Dfa;
};
