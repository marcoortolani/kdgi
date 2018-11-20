from gipy_lib import *

def f1(dfa1, dfa2):
	query = equivalence_query(dfa1, dfa2)
	print query
	query = equivalence_query(dfa2, dfa1)
	print query
	
	mat = neighbour_matching(dfa1, dfa2)
	n = mat[len(mat) - 1][0]
	print "Neighbour matching:"
	print n
	
	
def f2(dfa, oracle, max_length, alphabet):
	tp = 0
	fp = 0
	tn = 0
	fn = 0

	for i in range(0, max_length + 1) :
		for j in range(0, pow(len(alphabet), i)) :
			val = j
			word = []
			
			for k in range(0, i) :
				word.extend(alphabet[ val % len(alphabet) ])
				val = val / len(alphabet)
			
			if oracle.membership_query(word) != dfa.membership_query(word) :
				if dfa.membership_query(word) :
					fp = fp + 1
				else :
					fn = fn + 1
			else :
				if dfa.membership_query(word) :
					tp = tp + 1
				else :
					tn = tn + 1

	print "true positive:" + str(tp)
	print "true negative:" + str(tn)
	print "false positive:" + str(fp)
	print "false negative:" + str(fn)
	print

	
dataset = []

alphabet = ["a", "b", "c"]
rand_length = 7

#alphabet = ["a", "b"]
#rand_length = 9

rand = RandomOracle(rand_length, alphabet)

max_length = 4
for i in range(0, max_length + 1) :
	for j in range(0, pow(len(alphabet), i)) :
		val = j
		word = []
		
		for k in range(0, i) :
			word.extend(alphabet[ val % len(alphabet) ])
			val = val / len(alphabet)
		
		if rand.membership_query(word) :
			dataset.extend([word])
		
		#print word
dim = 0
for i in range(0, rand_length + 1):
	dim += pow(len(alphabet), i)

print "Complete language dimension:" + str(dim)
print "Dataset dimension:" + str(len(dataset))
print

silly = SillyOracle(dataset)

lr = TTTLearner_R(rand, alphabet)
lar = AngluinLearner_R(rand, alphabet)

ls = TTTLearner_S(silly, alphabet)
las = AngluinLearner_S(silly, alphabet)

#tttr = lr.learn()
#print "learned first language (ttt)"
#costs = lr.get_costs()
#print "equivalence cost:" + str(costs[0]) + "\tmembership cost:" + str(costs[1]) + "\tsymbol cost:" + str(costs[2]) + "\tredundant queries:" + str(costs[3])
#print "#states:" + str(tttr.get_num_states())
#print 

#lr.reset_costs()
#lr.set_opack(True)
#opackr = lr.learn()
#print "learned first language (opack)"
#costs = lr.get_costs()
#print "equivalence cost:" + str(costs[0]) + "\tmembership cost:" + str(costs[1]) + "\tsymbol cost:" + str(costs[2]) + "\tredundant queries:" + str(costs[3])
#print "#states:" + str(opackr.get_num_states())
#print

#angr = lar.learn()
#print "learned first language (ang)"
#costs = lar.get_costs()
#print "equivalence cost:" + str(costs[0]) + "\tmembership cost:" + str(costs[1]) + "\tsymbol cost:" + str(costs[2]) + "\tredundant queries:" + str(costs[3])
#print "#states:" + str(angr.get_num_states())
#print

ttts = ls.learn()
print "learned second language (ttt)"
costs = ls.get_costs()
print "equivalence cost:" + str(costs[0]) + "\tmembership cost:" + str(costs[1]) + "\tsymbol cost:" + str(costs[2]) + "\tredundant queries:" + str(costs[3])
print "#states:" + str(ttts.get_num_states())
print 

ls.reset_costs()
ls.set_opack(True)
opacks = ls.learn()
print "learned second language (opack)"
costs = ls.get_costs()
print "equivalence cost:" + str(costs[0]) + "\tmembership cost:" + str(costs[1]) + "\tsymbol cost:" + str(costs[2]) + "\tredundant queries:" + str(costs[3])
print "#states:" + str(opacks.get_num_states())
print 

angs = las.learn()
print "learned second language (ang)"
costs = las.get_costs()
print "equivalence cost:" + str(costs[0]) + "\tmembership cost:" + str(costs[1]) + "\tsymbol cost:" + str(costs[2]) + "\tredundant queries:" + str(costs[3])
print "#states:" + str(angs.get_num_states())
print

#tttr.print_dot("", "dfas/dot/ttt_rand.dot")
ttts.print_dot("", "dfas/dot/ttt_silly.dot")

#angr.print_dot("", "dfas/dot/ang_rand.dot")
angs.print_dot("", "dfas/dot/ang_silly.dot")

opacks.print_dot("", "dfas/dot/opack_silly.dot")

print "Results:"
print

print "ttts"
#f1(tttr, ttts)
print equivalence_query(rand, ttts)
print "ttts silly"
f2(ttts, silly, max_length, alphabet)
print "ttts rand"
f2(ttts, rand, rand_length, alphabet)

print "opacks"
#f1(opackr, opacks)
print equivalence_query(rand, opacks)
print "opacks silly"
f2(opacks, silly, max_length, alphabet)
print "opacks rand"
f2(opacks, rand, rand_length, alphabet)

print "angs"
#f1(angr, angs)
print equivalence_query(rand, angs)
print "angs silly"
f2(angs, silly, max_length, alphabet)
print "angs rand"
f2(angs, rand, rand_length, alphabet)
