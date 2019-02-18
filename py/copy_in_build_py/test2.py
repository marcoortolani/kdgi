from gipy_lib import *

def f1(dfa1, dfa2):
	query = equivalence_query(dfa1, dfa2)
	print(query)
	query = equivalence_query(dfa2, dfa1)
	print(query)
	
	mat = neighbour_matching(dfa1, dfa2)
	n = mat[len(mat) - 1][0]
	print("Neighbour matching:")
	print(n)
	
	
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

alphabet = ["a", "b"]
rand_length = 7

rand = RandomOracle(rand_length, alphabet)

angl = AngluinLearner_R(rand, alphabet)
tttOracle = angl.learn()

print(tttOracle.get_num_states())

tl = TTTLearner_A(tttOracle, alphabet)
al = AngluinLearner_A(tttOracle, alphabet)

ttt = tl.learn()
print("ttt learned")
print(tl.get_costs())

tl.reset_costs()
tl.set_opack(True)

opack = tl.learn()
print("opack learned")
print(tl.get_costs())

ang = al.learn()
print("ang learned")
print(al.get_costs())

print(equivalence_query(ttt, opack))
print(equivalence_query(opack, ang))
print(equivalence_query(ang, ttt))
