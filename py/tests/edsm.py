from gi_gipy import *
import os

learner = Edsm('./data/edsm_test.txt')

inferred_dfa = learner.run('./data/apt.txt')

inferred_dfa.print_dfa_dot('Inferred','./data/inferred_dfas/edsm_inferred.dot')

bashCommand = "dot -Tpdf ./data/inferred_dfas/edsm_inferred.dot > ./data/inferred_dfas/edsm_inferred.pdf"
os.system(bashCommand)