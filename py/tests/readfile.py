from gi_gipy import *

x=Dfa(Dfa.read_dfa_file('./struct_sim_experiments/2_symbols/automata/n5/n5_1.txt'))
h=x.get_dim_alphabet()
print ('Dimensione dell\'alfabeto del Tomita15 letto da file: ')
print (h)
x.print_dfa_ttable("Tomita15")
print (x.membership_query(["a"]))
print (x.is_accepting(3))
