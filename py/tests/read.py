from gi_gipy import *

dfa_file1 = '../data/automata/thesis/hierarchical/palermo_ustica.txt'
dfa_file2 = '../data/automata/thesis/hierarchical/palermo_castellammare.txt'


dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)