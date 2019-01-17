
import pandas as pd
from collections import Counter
import pyprind
import string
import numpy as np

def csv2intlist(X_label, y_label, filename='oracle1.csv', testTrainRatio=0.5, alphabet=['', 'a', 'b']):
	df = pd.read_csv(filename, encoding='utf-8').sample(frac=1, replace=True).reset_index(drop=True)
	length = int(len(df.loc[:, X_label].values) * testTrainRatio)
	X_train = []
	for sample in df.loc[:length, X_label].values:
		syms = sample.split(" ")
		vals = [alphabet.index(el) for el in syms]
		X_train = X_train + [vals]
		
	y_train = df.loc[:length, y_label].values

	X_test = []
	for sample in df.loc[length + 1:, X_label].values:
		syms = sample.split(" ")
		vals = [alphabet.index(el) for el in syms]
		X_test = X_test + [vals]
	y_test = df.loc[length + 1:, y_label].values
	
	return X_train, y_train, X_test, y_test
