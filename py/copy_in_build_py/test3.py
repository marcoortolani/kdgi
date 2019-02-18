from gipy_lib import *

def gen_dataset_from_oracle(oracle, length, alphabet) :
	dataset = []
	for i in range(0, length + 1) :
		for j in range(0, pow(len(alphabet), i)) :
			val = j
			word = []
			
			for k in range(0, i) :
				word.extend(alphabet[ int(val % len(alphabet)) ])
				val = val / len(alphabet)
			
			if oracle.membership_query(word) :
				dataset.extend([word])
				
	return dataset

alphabet1 = ["a", "b", "c", "d", 'e']
alphabet2 = ["a", "b", "c"]
len1 = 3
len2 = 5

rand1 = RandomOracle(len1, alphabet1)
rand2 = RandomOracle(len2, alphabet2)

dataset1 = gen_dataset_from_oracle(rand1, len1, alphabet1)
dataset2 = gen_dataset_from_oracle(rand2, len2, alphabet2)

silly_lang1_per50 = SillyOracle(dataset1, 50)
silly_lang1_per20 = SillyOracle(dataset1, 20)
silly_lang2_per50 = SillyOracle(dataset2, 50)
silly_lang2_per20 = SillyOracle(dataset2, 20)

lt1 = TTTLearner_S(silly_lang1_per20, alphabet1)
lt2 = TTTLearner_S(silly_lang2_per20, alphabet2)

la1 = AngluinLearner_S(silly_lang1_per50, alphabet1)
la2 = AngluinLearner_S(silly_lang2_per50, alphabet2)

ttt1 = lt1.learn()
print(ttt1.get_num_states())
ang1 = la1.learn()
print(ang1.get_num_states())
ttt2 = lt2.learn()
print(ttt2.get_num_states())
ang2 = la2.learn()
print(ang2.get_num_states())

vec = [ttt1, ang1, ttt2, ang2]

for i in range(0, 4) :
	row = ""
	for j in range(0, 4) :
		mat = neighbour_matching(vec[i], vec[j])
		n = mat[len(mat) - 1][0]
		row += str(n) + "\t"
	print(row)
