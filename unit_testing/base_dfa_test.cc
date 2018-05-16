#include <iostream>
#include <numeric>
#include <vector>
#include <string>
using namespace std;
#include "gtest/gtest.h"

#include "testDfa.h"


class BaseDfaTest : public ::testing::Test {
protected:

	BaseDfaTest() {
		// You can do set-up work for each test here.
	}

	virtual ~BaseDfaTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
		reference->set_ttable_from_sequence(sequence);
        subject->set_ttable_from_sequence(sequence);
	}

	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Objects declared here can be used by all tests in the test case for Project1.
	vector<string> alph={"0","1","2"};
	const vector<int> sequence = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0};
	TestDfa* reference = new TestDfa(5,alph);
    TestDfa* subject = new TestDfa(5,alph);

};


TEST_F(BaseDfaTest, getDimAlphabet){
	EXPECT_EQ(3,reference->get_dim_alphabet());
}

TEST_F(BaseDfaTest, getTransition){
    EXPECT_EQ(3,reference->get_ttable(1,"0"));
}

TEST_F(BaseDfaTest, setAcceptingState){
    reference->set_accepting_state(1);
    EXPECT_EQ(1,reference->get_accepting_states()[1]);
    reference->set_rejecting_state(1);
    EXPECT_EQ(0,reference->get_accepting_states()[1]);
}

TEST_F(BaseDfaTest, getAlphabet){
    const vector<string> alph={"0","1","2"};
    vector<string> alph_test=reference->get_alphabet();
    bool equal=true;
    int i;
    for(i=0;i<reference->get_dim_alphabet();i++){
        if(alph[i]!=alph_test[i])
            equal=false;
    }
    EXPECT_EQ(1,equal);
}


TEST_F(BaseDfaTest, getNumStates){
    EXPECT_EQ(5,reference->get_num_states());
}


TEST_F(BaseDfaTest, getStartState){
    EXPECT_EQ(0,reference->get_start_state());
}

TEST_F(BaseDfaTest, getAcceptingStates){
    vector<int> acc={0,0,0,1,0};
    vector<int> acc_ref=reference->get_accepting_states();
    bool flag=true;
    for(int i=0; i<reference->get_num_states();++i)
        if(acc[i]!=acc_ref[i]){
            flag=false;
            break;
        }
    EXPECT_EQ(1,flag);
}


TEST_F(BaseDfaTest, readDfaFile){
    Dfa* test=nullptr; 
    test=new Dfa(test->read_dfa_file("../unit_testing/data/tomita15.txt"));
    vector<map<string,int>> ttable_test=test->get_ttable();
    vector<map<string,int>> ttable_ref=reference->get_ttable();
    bool equal=true;
    int i;
    for(i=0;i<reference->get_num_states();i++){
        if(reference->is_accepting(i)!=test->is_accepting(i)){
            cout<<"stato incriminato:"<<i<<endl;
            equal=false;
            break;
        }
        for(string sym : reference->get_alphabet())
            if(ttable_test[i][sym]!=ttable_ref[i][sym]){
                cout<<"i="<<i<<endl<<"sym"<<sym<<endl;
                cout<<"ttable_test[i][sym]="<<ttable_test[i][sym]<<endl<<"ttable_ref[i][sym]="<<ttable_ref[i][sym]<<endl;
                equal=false;
                break;
            }
        if(!equal)
            break;
    }
    EXPECT_EQ(1,equal);
}

TEST_F(BaseDfaTest, membershipQuery){
    bool flag=true;
    vector<string> try1={"0","0"};  //stringa 00 accettata
    flag=reference->membership_query(try1);
    vector<string> try2={"0","2","0"};  //stringa 020 accettata
    flag=reference->membership_query(try2);
    vector<string> try3={"2"};  //non accettata
    flag=!(reference->membership_query(try3));
    EXPECT_EQ(1,flag);
}

TEST_F(BaseDfaTest, operatorEqual){
    TestDfa* test = new TestDfa();
    *test = *reference;
    vector<map<string,int>> ttable_test=test->get_ttable();
    vector<map<string,int>> ttable_ref=reference->get_ttable();
    bool equal=true;
    int i;
    for(i=0;i<reference->get_num_states();i++){
        if(reference->is_accepting(i)!=test->is_accepting(i)){
            cout<<"stato incriminato:"<<i<<endl;
            equal=false;
            break;
        }
        for(string sym : reference->get_alphabet())
            if(ttable_test[i][sym]!=ttable_ref[i][sym]){
                cout<<"i="<<i<<endl<<"sym"<<sym<<endl;
                cout<<"ttable_test[i][sym]="<<ttable_test[i][sym]<<endl<<"ttable_ref[i][sym]="<<ttable_ref[i][sym]<<endl;
                equal=false;
                break;
            }
        if(!equal)
            break;
    }
    EXPECT_EQ(1,equal);
}

TEST_F(BaseDfaTest, operatorEqualEqual){
    bool flag=true;
    flag=(*reference==*subject);
    reference->set_ttable_entry(0,"0",0);
    flag=!(*reference==*subject);
    EXPECT_EQ(1,flag);
}


TEST_F(BaseDfaTest, unionDfa){
    TestDfa* test = new TestDfa(3,alph,2);
    const vector<int> sequence2 = {0, 1, 2, 0, 0, 1, 2, 1, 0, 1, 2, 0};
    test->set_ttable_from_sequence(sequence2);
    Dfa* uni=reference->unionDFA(test);
    TestDfa* uni_ref = new TestDfa(8,alph,0);
    const vector<int> sequence3 = {1, 2, 4, 0, 3, 4, 1, 0, 4, 3, 2, 0, 4, 4, 4, 1, 4, 4, 4, 0, 5, 6, 7, 0, 5, 6, 7, 1, 5, 6, 7, 0};
    uni_ref->set_ttable_from_sequence(sequence3);
    vector<map<string,int>> ttable_test=uni->get_ttable();
    vector<map<string,int>> ttable_ref=uni_ref->get_ttable();
    bool equal=true;
    int i;
    for(i=0;i<uni->get_num_states();i++){
        if(uni->is_accepting(i)!=uni_ref->is_accepting(i)){
            cout<<"stato incriminato:"<<i<<endl;
            equal=false;
            break;
        }
        for(string sym : uni->get_alphabet())
            if(ttable_test[i][sym]!=ttable_ref[i][sym]){
                cout<<"i="<<i<<endl<<"sym"<<sym<<endl;
                cout<<"ttable_test[i][sym]="<<ttable_test[i][sym]<<endl<<"ttable_ref[i][sym]="<<ttable_ref[i][sym]<<endl;
                equal=false;
                break;
            }
        if(!equal)
            break;
    }
    EXPECT_EQ(1,equal);
}

/*
TEST_F(BaseDfaTest, equivalenceQuery){
    bool flag=true;
    Dfa* test=nullptr; 
    test=new Dfa(test->read_dfa_file("../unit_testing/data/tomita15.txt"));
    vector<string> ok={"a"};
    flag=reference->equivalence_query(test, ok);
    EXPECT_EQ(1,flag);
}
*/
