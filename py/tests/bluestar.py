from gi_gipy import *
import os

learner = BlueStar('./data/bluestar_test.txt')

inferred_dfa = learner.run('./data/apt.txt')

inferred_dfa.print_dfa_dot('Inferred','./data/inferred_dfas/bluestar_inferred.dot')

bashCommand = "dot -Tpdf ./data/inferred_dfas/bluestar_inferred.dot > ./data/inferred_dfas/bluestar_inferred.pdf"
os.system(bashCommand)

learner_weights = BlueStar('./data/bluestar_test_weighted.txt')

inferred_dfa_weights = learner_weights.run('./data/apt.txt')

inferred_dfa_weights.print_dfa_dot('Inferred','./data/inferred_dfas/bluestar_weights_inferred.dot')

bashCommand = "dot -Tpdf ./data/inferred_dfas/bluestar_weights_inferred.dot > ./data/inferred_dfas/bluestar_weights_inferred.pdf"
os.system(bashCommand)