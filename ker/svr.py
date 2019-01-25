import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import alutils as ut

from keras.preprocessing import sequence

from sklearn.svm import SVR
from sklearn.preprocessing import StandardScaler

class Tree:
	def __init__(self, left, right, svrNscalers):
		self.left = left
		self.right = right
		self.svr = svrNscalers[0]
		self.sc_X = svrNscalers[1]
		self.sc_y = svrNscalers[2]
        
	def add_node(self, lr, svr, value):
		if lr != "left" and lr != "right":
			raise ValueError('In function add_node of class Tree: parameter lr must be either "left" or "right".')
		
		if lr == "left":
			node = Tree(self.left, value, svr)
			self.left = node
			
		if lr == "right":
			node = Tree(self.right, value, svr)
			self.right = node
	
	def branch_down(self, x):
		y_pred = self.sc_y.inverse_transform(self.svr.predict(self.sc_X.fit_transform([x])))
		print(y_pred)
		

def fit_svm(X, yo, accuracy, max_length, max_iter):
	sc_X = StandardScaler()
	sc_y = StandardScaler()
	regressor = SVR(kernel='rbf', gamma='auto')
	print(regressor.get_params())
	X = sequence.pad_sequences(X, maxlen=max_length)
	yo = np.array(yo).reshape(-1, 1)
	X = sc_X.fit_transform(X)
	y = sc_y.fit_transform(yo)
	
	acc = 0
	it = 0
	while acc < accuracy and it < max_iter:
		it = it + 1
		#regressor.fit(X, y)
		regressor.fit(X, np.ravel(y,order='C'))
		
		y_pred = sc_y.inverse_transform(regressor.predict(X))
		
		count = 0
		for i in range(len(X)):
			if(abs(yo[i][0] - y_pred[i]) < 0.5):
				count = count + 1
		
		acc = float(count) / float(len(X))
	print(regressor.score(X, yo))
	return regressor, sc_X, sc_y

"""
max_length = 20

X_train_o, y_train_o, X_test_o, y_test_o = ut.csv2intlist("review", "sentiment", filename="csv/oracle6.csv", testTrainRatio=0.75)
X_test_o = sequence.pad_sequences(X_test_o, maxlen=max_length)
sc_X = StandardScaler()
sc_y = StandardScaler()
regressor = fit_svm(X_test_o, y_test_o, sc_X, sc_y, 0.70, max_length, 100)

y_pred = sc_y.inverse_transform(regressor.predict(sc_X.fit_transform(X_test_o)))

count = 0
for i in range(len(X_test_o)):
	if(abs(y_test_o[i] - y_pred[i]) > 0.5):
		print(X_test_o[i])
		count = count + 1

print("count:" + str(count))
print("tot samples:" + str(len(X_test_o)))
"""

max_length = 5

x1 = [0.0, 1.0, 0.6, 0.2, 0.84]
x2 = [1.0, 0.62, 0.84, 0.11, 0.0]

X = [x1, x2]
y = [0, 1]
svrNscalers = fit_svm(X, y, 0.70, max_length, 10)

svr = svrNscalers[0]
print(svr.score(X, y))

sc_X = svrNscalers[1]
sc_y = svrNscalers[2]

"""
T = Tree(0, 1, svrNscalers)
y_pred = sc_y.inverse_transform(svr.predict(sc_X.fit_transform([x1])))
print(y_pred)
"""
#T.branch_down(x1)

#svr = fit_svm(X_test_o[1:3], y_test_o[1:3], sc_X, sc_y, 0.70, max_length, 1)

#T.add_node("left", svr, 2)

#print(T)
#print(T.left)
#print(T.right)
