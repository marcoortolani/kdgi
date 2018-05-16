//******** Thin-wrapper to do unit-testing of abstract classes ********
#include "bluefringe.h"

class TestBlueFringe : public BlueFringe{

 friend class BlueFringeTest;

  FRIEND_TEST(BlueFringeTest, readSamples);

public:
  using BlueFringe::BlueFringe;
};
