import numpy as np
import random
np.random.seed(0)

def random_word(max_len, alphabet):
	length = random.randint(0, max_len)
	word = []
	for i in range(length):
		index = random.randint(0, len(alphabet) - 1)
		word = word + [alphabet[index]]
		
	return word

def odd_a(word):
	odd_a = False
	for sym in word:
		if sym == "a":
			odd_a = not odd_a
	return odd_a

def gen_dataset(num_samples, max_len, alphabet):
	X = []
	y = []
	
	for i in range(num_samples):
		X = X + [random_word(max_len, alphabet)]
		y = y + [odd_a(X[-1])]
	
	return X, y
