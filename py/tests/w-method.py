from gi_gipy import *

dfa_file1 = '../data/automata/thesis/hierarchical/altro/sq9/pa-ag.txt'
dfa_file2 = '../data/automata/thesis/hierarchical/altro/sq9/pa-ag_2.txt'
#dfa_file1 = '../data/automata/tomita15.txt'
#dfa_file2 = '../data/automata/tomita15traslato.txt'

dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)

testset = dfa1.get_w_method_test_set(dfa2)

testset2 = dfa2.get_w_method_test_set(dfa1)

dfa1.print_w_method(dfa1.get_w_method_statistics(testset,dfa2))
dfa2.print_w_method(dfa2.get_w_method_statistics(testset2,dfa1))


