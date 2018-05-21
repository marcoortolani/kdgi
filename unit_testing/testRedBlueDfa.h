#ifndef TEST_REDBLUEDFA_H_
#define TEST_REDBLUEDFA_H_

#include "redbluedfa.h"

class TestRedBlueDfa : public RedBlueDfa {
    friend class RedBlueDfaTest;
    FRIEND_TEST(RedBlueDfaTest, setAcceptingState);
    public:
        using RedBlueDfa::RedBlueDfa;
};

#endif