from gipy_lib import *

dataset = []

alphabet = ["a", "b"]
rand_length = 8
rand = RandomOracle(rand_length + 1, alphabet)

max_length = 6
for i in range(0, max_length + 1) :
	for j in range(0, pow(len(alphabet), i) + 1) :
		val = j
		word = []
		
		for k in range(0, i) :
			word.extend(alphabet[ val % len(alphabet) ])
			val = val / len(alphabet)
		
		if rand.membership_query(word) :
			dataset.extend([word])
			print(word)

silly = SillyOracle(dataset)

lr = TTTLearner_R(rand, alphabet)
ls = TTTLearner_S(silly, alphabet)

tttr = lr.learn()

print "learned first language"

ttts = ls.learn()

print "learned second language"

tttr.print_dot("", "dfas/dot/rand.dot")
ttts.print_dot("", "dfas/dot/silly.dot")

print "Results:"

query = equivalence_query(tttr, ttts)
print query
query = equivalence_query(ttts, tttr)
print query

n = neighbour_matching(tttr, ttts)
print "Neighbour matching:"
print n
