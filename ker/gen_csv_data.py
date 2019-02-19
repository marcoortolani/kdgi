from gipy_lib import *
import csv

def gen_dataset_from_oracle(oracle, length):
	dataset = []
	alphabet = oracle.get_alphabet()
	for i in range(0, length + 1) :
		for j in range(0, pow(len(alphabet), i)):
			val = j
			word = []
			
			for k in range(0, i) :
				word.extend(alphabet[ int(val % len(alphabet)) ])
				val = val / len(alphabet)
			
			row = [word, oracle.membership_query(word)]
			dataset.extend([row])
				
	return dataset
		
def write_data(oracle, length, filename):
	dataset = gen_dataset_from_oracle(oracle, length)
	with open(filename, mode='w') as data_file:
		data_writer = csv.writer(data_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
		data_writer.writerow(['word', 'acceptance'])
		data_writer = csv.writer(data_file, delimiter=',', quotechar='"', quoting=csv.QUOTE_NONNUMERIC)
		
		for row in dataset:
			word = row[0]
			s = ' '.join(word)
			if(s == ''):
				s = ' '
			t = 0
			if(row[1]):
				t = 1
			data_writer.writerow([s, t])

#c = ConcreteDfa.read_dfa_file("data/_odd_a.txt")
#write_data(c, 8, 'csv/oracle.csv')
