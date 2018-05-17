from gi_gipy import *

dfa_file1 = '../data/tomita12.txt'
dfa_file2 = '../data/tomita14.txt'

dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)

dfa1.print_dfa_ttable("dfa1")
dfa2.print_dfa_ttable("dfa2")

dfa1.dfa_similarity(dfa2)