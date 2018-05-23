from gi_gipy import *

dfa_file1 = '../data/tomita1.txt'
dfa_file2 = '../data/tomita2.txt'

dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)

testset = dfa1.get_w_method_test_set(dfa2)

dfa1.print_w_method(dfa1.get_w_method_statistics(testset,dfa2))


