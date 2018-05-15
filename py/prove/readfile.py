from gi_gipy import *

x=Dfa()
x.read_dfa_file('../data/tomita15.txt')
h=x.get_dim_alphabet()
print 'Dimensione dell\'alfabeto del Tomita15 letto da file: ' 
print h