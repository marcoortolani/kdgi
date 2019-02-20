from gipy_lib import *

import sys
if sys.version_info[0] < 3:
	filename = "model_2.7"
else:
	filename = "model6b"

print(filename)

alphabet = ['a', 'b']
layer = 2
max_build = 35

rnnOracle = RNNOracle(filename, alphabet, layer, max_build)

ang = AngluinLearner_RNN(rnnOracle, alphabet)

opack = TTTLearner_RNN(rnnOracle, alphabet)
opack.set_opack(True)

ttt = TTTLearner_RNN(rnnOracle, alphabet)

ang_dfa = ang.learn()
print("ang learned")

opack_dfa = opack.learn()
print("opack learned")

ttt_dfa = ttt.learn()
print("ttt learned")

print("ang")
ang_dfa.print_dfa()
print()

print("opack")
opack_dfa.print_dfa()
print()

print("ttt")
ttt_dfa.print_dfa()
print()

query = equivalence_query(ttt_dfa, ang_dfa)

if not query[0] :
	print("\t" + str(query[1]))
	print("\tang:" + str(ang_dfa.membership_query(query[1])))
	print("\tttt:" + str(ttt_dfa.membership_query(query[1])))
	print("\toracle:" + str(rnnOracle.membership_query(query[1])))
