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
	
	
def fmeasure(dfa, oracle, max_length, alphabet):
	tp = 0.0
	fp = 0.0
	tn = 0.0
	fn = 0.0

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

	precision = tp / (tp + fp)
	recall = tp / (tp + fn)
	f_measure = 2 * (precision * recall) / (precision + recall)
	#print "precision:" + str(precision)
	#print "recall:" + str(recall)
	return f_measure

def test3(alphabet, rand_length, max_length) :
	if max_length > rand_length :
		max_length = rand_length
	
	dim = 0
	for i in range(0, rand_length + 1):
		dim += pow(len(alphabet), i)

	print "Complete language dimension:" + str(dim)
	print
	print "\tttt\t\topack\t\tang"

	for silly_length in range(2, max_length + 1) :
		dataset = []
		rand = RandomOracle(rand_length, alphabet)

		for i in range(0, silly_length + 1) :
			for j in range(0, pow(len(alphabet), i)) :
				val = j
				word = []
				
				for k in range(0, i) :
					word.extend(alphabet[ val % len(alphabet) ])
					val = val / len(alphabet)
				
				if rand.membership_query(word) :
					dataset.extend([word])

		silly = SillyOracle(dataset)

		lr = TTTLearner_R(rand, alphabet)
		lar = AngluinLearner_R(rand, alphabet)

		ls = TTTLearner_S(silly, alphabet)
		las = AngluinLearner_S(silly, alphabet)

		ttts = ls.learn()

		ls.reset_costs()
		ls.set_opack(True)
		opacks = ls.learn()

		angs = las.learn()

		fttt = fmeasure(ttts, rand, rand_length, alphabet)

		fopack = fmeasure(opacks, rand, rand_length, alphabet)

		fang = fmeasure(angs, rand, rand_length, alphabet)

		print str(len(dataset)) + "\t" + str(fttt) + "\t" + str(fopack) + "\t" + str(fang)


test3(['a', 'b', 'c', 'd', 'e'], 4, 6)
