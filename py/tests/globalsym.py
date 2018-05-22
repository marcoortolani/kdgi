from gi_gipy import *

dfa_file1 = '../data/tomita9.txt'
dfa_file2 = '../data/tomita15.txt'

dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)

sim=dfa1.dfa_similarity(dfa2,1)