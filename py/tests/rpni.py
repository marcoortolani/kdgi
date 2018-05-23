from gi_gipy import *
import os

learner = Rpni('./data/rpni_test.txt')

inferred_dfa = learner.run('./data/apt.txt')

inferred_dfa.print_dfa_dot('Inferred','./data/inferred_dfas/rpni_inferred.dot')

bashCommand = "dot -Tpdf ./data/inferred_dfas/rpni_inferred.dot > ./data/inferred_dfas/rpni_inferred.pdf"
os.system(bashCommand)