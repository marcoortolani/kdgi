from gipy_lib import *

filename = "model6b"
alphabet = ['a', 'b']
layer = 2
max_build = 35

rnnOracle = RNNOracle(filename, alphabet, layer, max_build)

ang = AngluinLearner_RNN(rnnOracle, alphabet)

opack = TTTLearner_RNN(rnnOracle, alphabet)
opack.set_opack(True)

ttt = TTTLearner_RNN(rnnOracle, alphabet)

ang_dfa = ang.learn()

opack_dfa = opack.learn()

ttt_dfa = ttt.learn()

print(ang_dfa.get_num_states())
print(opack_dfa.get_num_states())
print(ttt_dfa.get_num_states())

query = equivalence_query(ttt_dfa, ang_dfa)

if not query[0] :
	print("\t" + str(query[1]))
	print("\tang:" + str(ang_dfa.membership_query(query[1])))
	print("\tttt:" + str(ttt_dfa.membership_query(query[1])))
	print("\toracle:" + str(rnnOracle.membership_query(query[1])))
