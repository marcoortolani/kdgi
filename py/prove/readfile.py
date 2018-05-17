from gi_gipy import *

x=Dfa(Dfa.read_dfa_file('../data/tomita15.txt'))
h=x.get_dim_alphabet()
print ('Dimensione dell\'alfabeto del Tomita15 letto da file: ')
print (h)
x.print_dfa_ttable("Tomita15")
print (x.membership_query(["a"]))
print (x.is_accepting(3))