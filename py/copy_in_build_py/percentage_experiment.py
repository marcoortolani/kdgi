from gipy_lib import *

v = ["25","30","45","115","140","155"]
alphabet = ['a', 'b']
layer = 2
max_build = 35
counterexamples = [['a', 'a', 'a', 'b', 'a', 'a', 'a', 'a'],['a', 'a', 'a', 'a', 'a', 'a']]#tomita4 OK
c = ConcreteDfa.read_dfa_file("data/tomita4.txt")
c.print_dfa()

col = 0

for n in v:
	filename = "tomita4_" + n
	print(filename)
	
	rnnOracle = RNNOracle(filename, alphabet, layer, max_build, counterexamples)
	for cntr in counterexamples:
		if rnnOracle.membership_query(cntr):
			print ("errore")

	ang = AngluinLearner_RNN(rnnOracle, alphabet)
	ang_dfa = ang.learn()
	print("ang learned")
	print(ang.get_costs())
	print()


	rnnOracle = RNNOracle(filename, alphabet, layer, max_build, counterexamples)
	opack = TTTLearner_RNN(rnnOracle, alphabet)
	opack.set_opack(True)
	opack_dfa = opack.learn()
	print("opack learned")
	print(opack.get_costs())
	print()

	rnnOracle = RNNOracle(filename, alphabet, layer, max_build, counterexamples)
	ttt = TTTLearner_RNN(rnnOracle, alphabet)
	ttt_dfa = ttt.learn()
	print("ttt learned")
	print(ttt.get_costs())
	print()

	print("ang")
	ang_dfa.print_dfa()
	query = equivalence_query(c, ang_dfa)
	print(query)
	print()

	print("opack")
	opack_dfa.print_dfa()
	query = equivalence_query(c, opack_dfa)
	print(query)
	print()

	print("ttt")
	ttt_dfa.print_dfa()
	query = equivalence_query(c, ttt_dfa)
	print(query)
	print()
	
	print("ang")
	print((neighbour_matching(ang_dfa, c)[-1][0] + neighbour_matching(c, ang_dfa)[-1][0])/2)
	print()
	print("opack")
	print((neighbour_matching(opack_dfa, c)[-1][0] + neighbour_matching(c, opack_dfa)[-1][0])/2)
	print()
	print("ttt")
	print((neighbour_matching(ttt_dfa, c)[-1][0] + neighbour_matching(c, ttt_dfa)[-1][0])/2)
