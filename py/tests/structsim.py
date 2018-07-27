from gi_gipy import *

dfa_file1 = './struct_sim_experiments/automata/n10/n10_1.txt'
dfa_file2 = './struct_sim_experiments/automata/n10/n10_2.txt'

dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)

# neighbour_matching_structural_similarity(ConcreteDfa* subject_dfa, double eps=0.0001, bool color=false)
struct_sim_matrix = dfa1.neighbour_matching_structural_similarity(dfa2, 0.000001)

dfa1.print_structural_similarity(struct_sim_matrix)
