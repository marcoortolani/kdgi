from gi_gipy import *
import os
import random


for n in range(10, 51):
    
    for automata_number in range(1, 11):

        dfa_name = 'n' + str(n) + '_' + str(automata_number)

        file_path = './struct_sim_experiments/automata/n' + str(n) + '/' + dfa_name + '.txt'

        seed = n * automata_number * random.randint(1,101)

        Dfa.abbadingo_rand(n, seed, file_path)

        x=Dfa(Dfa.read_dfa_file(file_path))

        dot_path = './struct_sim_experiments/dot/n' + str(n) + '/' + dfa_name + '.dot'

        x.print_dfa_dot(dfa_name, dot_path)

        pdf_path = './struct_sim_experiments/automata_pics/n' + str(n) + '/' + dfa_name + '.pdf'

        bashCommand = 'dot -Tpdf '+ dot_path + ' > ' + pdf_path

        os.system(bashCommand)