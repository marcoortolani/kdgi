from gipy_lib import *

"This is a code to let a bug surface.
In order to do this go into TTTDfa.cc
in the TTTDfa :: know_phrase_for_sure function and make sure
that the following lines are uncommented:
	
	auto it = lost_queries_.find(phrase);
	if(it != lost_queries_.end()){
		truth = it->second;
		return true;
	}
	
	return false;
	
then build the library and run this code in the kdgi/build/py folder."

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
	return f_measure
	
dataset = []

alphabet = ["a", "b", "c"]
rand_length = 7

rand = RandomOracle(rand_length, alphabet)

def test4(percentage) :

	max_length = 5
	for i in range(0, max_length + 1) :
		for j in range(0, pow(len(alphabet), i)) :
			val = j
			word = []
			
			for k in range(0, i) :
				word.extend(alphabet[ val % len(alphabet) ])
				val = val / len(alphabet)
			
			if rand.membership_query(word) :
				dataset.extend([word])

	silly = SillyOracle(dataset, percentage)

	lt = TTTLearner_S(silly, alphabet)
	la = AngluinLearner_S(silly, alphabet)

	ttt = lt.learn()
	lt.set_opack(True)
	lt.reset_costs()
	
	opack = lt.learn()
	
	ang = la.learn()

	print str(percentage) + "\t" + str(fmeasure(ttt, silly, max_length, alphabet)) + "\t" + str(fmeasure(opack, silly, max_length, alphabet)) + "\t" + str(fmeasure(ang, silly, max_length, alphabet))
	
	ttt.print_dot("", "dfas/dot/ttt_" + str(percentage) + ".dot")
	opack.print_dot("", "dfas/dot/opack_" + str(percentage) + ".dot")
	ang.print_dot("", "dfas/dot/ang_" + str(percentage) + ".dot")
	
print "\tttt\t\topack\t\tang"

for percentage in range(0, 101, 10) :
	test4(percentage)
