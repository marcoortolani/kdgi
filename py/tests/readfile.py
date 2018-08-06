from gi_gipy import *

for n_symbols in range(8, 9):

    symbol_path = './new_struct_sim_experiments/' + str(n_symbols) + '_symbols'

    for n in range(2, 50):
        
        for automata_number in range(1, 11):

            dfa_name = 'n' + str(n) + '_' + str(automata_number)

            dir_path = symbol_path + '/automata/n' + str(n)

            file_path = dir_path + '/' + dfa_name + '.txt'

            x=Dfa(Dfa.read_dfa_file(file_path))