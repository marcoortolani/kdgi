from gi_gipy import *

x=Dfa()
h=x.get_dim_alphabet()
print 'Dimensione dell\'alfabeto del Dfa vuoto: ' 
print h


dim_states=3
dim_alphabet=1
alph='1'
y=Dfa(dim_states,dim_alphabet,alph)
g=y.get_dim_alphabet()
print 'Dimensione dell\'alfabeto del Dfa con 3 stati e alfabeto solo "a": ' 
print g 

dim_states=3
dim_alphabet=2
alph2="xyz"
z=Dfa(dim_states,dim_alphabet,alph2,0)
t=z.get_dim_alphabet()
print 'Dimensione dell\'alfabeto del Dfa con 3 stati e alfabeto solo "a b": ' 
print t 
