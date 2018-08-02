from gi_gipy import *
import os
import random

# alphabet size
n_symbols = 2

symbol_path = './struct_sim_experiments_test/' + str(n_symbols) + '_symbols'

if not os.path.exists(symbol_path):
    os.makedirs(symbol_path)

results_path = symbol_path + '/results'

if not os.path.exists(results_path):
    os.makedirs(results_path)

intra_n_path = results_path + '/intra_n'

for n in range(2, 51):
    
    for automata_number in range(1, 11):

        dfa_name = 'n' + str(n) + '_' + str(automata_number)

        dir_path = symbol_path + '/automata/n' + str(n)

        if not os.path.exists(dir_path):
            os.makedirs(dir_path)

        file_path = dir_path + '/' + dfa_name + '.txt'

        seed = n * automata_number * random.randint(1,101)

        Dfa.abbadingo_rand(n, seed, n_symbols, file_path)

        x=Dfa(Dfa.read_dfa_file(file_path))

        dir_path2 = symbol_path + '/dot/n' + str(n)

        if not os.path.exists(dir_path2):
            os.makedirs(dir_path2)

        dot_path = dir_path2 + '/' + dfa_name + '.dot'

        x.print_dfa_dot(dfa_name, dot_path)

        dir_path3 = symbol_path + '/automata_pics/n' + str(n)

        if not os.path.exists(dir_path3):
            os.makedirs(dir_path3)

        pdf_path = dir_path3 + '/' + dfa_name + '.pdf'

        bashCommand = 'dot -Tpdf '+ dot_path + ' > ' + pdf_path

        os.system(bashCommand)