from gi_gipy import *

# dfa_file1 = './struct_sim_experiments/2_symbols/automata/n5/n5_1.txt'
# dfa_file2 = './struct_sim_experiments/2_symbols/automata/n2/n2_1.txt'
dfa_file1 = '../data/automata/_2_instance_of_aab.txt'
dfa_file2 = '../data/automata/tomita15.txt'


dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)

# dfa1.print_dfa_ttable("ref")
# dfa2.print_dfa_ttable("sub")

# neighbour_matching(ConcreteDfa* subject_dfa, double eps=0.0001, bool color=false)
struct_sim_matrix = dfa1.neighbour_matching(dfa2)

dfa1.print_structural_similarity(struct_sim_matrix)
