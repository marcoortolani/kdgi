from gi_gipy import *

dfa_file1 = '../data/tomita1.txt'
dfa_file2 = '../data/tomita2.txt'

dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)

struct_sim_matrix = dfa1.neighbour_matching_structural_similarity(dfa2)

dfa1.print_structural_similarity(struct_sim_matrix,dfa2.get_num_states())