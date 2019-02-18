from gipy_lib import *

for i in range(1, 16) :
	
	if i == 9 or i == 15:
		continue
		
	name = "tomita" + str(i)
	print(name)
	c = ConcreteDfa.read_dfa_file("data/" + name + ".txt")
	c1 = ConcreteDfa.read_dfa_file("data/tomita2.txt")
	
	lc = AngluinLearner_C(c, c.get_alphabet())
	ac = lc.learn()
	print(lc.get_costs())
	
	tttl = TTTLearner_C(c, c.get_alphabet())
	ttt = tttl.learn()
	print(tttl.get_costs())

	tttl.reset_costs()
	tttl.set_opack(True)
	opack = tttl.learn()
	print(tttl.get_costs())

	cntr = equivalence_query(c, c1)
	print("Equivalence query ttt")
	print(cntr)

	cntr = equivalence_query(c, c1)
	print("Equivalence query angluin")
	print(cntr)

	cntr = equivalence_query(c1, opack)
	print("Equivalence query opack")
	print(cntr)
	
	mat = neighbour_matching(c, opack)
	n = mat[len(mat) - 1][0]
	print(n)
	mat = neighbour_matching(opack, ttt)
	n = mat[len(mat) - 1][0]
	print(n)
	mat = neighbour_matching(ttt, c)
	n = mat[len(mat) - 1][0]
	print(n)

	ttt.print_dot("", "dfas/dot/" + name + "_ttt.dot")
	opack.print_dot("", "dfas/dot/" + name + "_opack.dot")
	ac.print_dot("", "dfas/dot/" + name + "_ang.dot")
