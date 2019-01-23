import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import alutils as ut

from keras.preprocessing import sequence

"""
dataset = pd.read_csv('csv/oracle6.csv')
X = dataset.iloc[1:, 0].values
y = dataset.iloc[1:, 1].values

from sklearn.model_selection import train_test_split
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2)
"""

max_length = 20

X_train, y_train, X_test, y_test = ut.csv2intlist("review", "sentiment", filename="csv/oracle6.csv", testTrainRatio=0.75)
X_train = sequence.pad_sequences(X_train, maxlen=max_length)
X_test = sequence.pad_sequences(X_test, maxlen=max_length)
y_train = np.array(y_train).reshape(-1, 1)
y_test = np.array(y_test).reshape(-1, 1)
#print(type(X_train))
#print(X_train)
#print(y_train)

from sklearn.preprocessing import StandardScaler
sc_X = StandardScaler()
X_train = sc_X.fit_transform(X_train)
X_test = sc_X.transform(X_test)
sc_y = StandardScaler()
y_train = sc_y.fit_transform(y_train)

from sklearn.svm import SVR
regressor = SVR(kernel='rbf')
regressor.fit(X_train, y_train)

y_pred = regressor.predict(X_train)
y_pred =  np.array(y_pred).reshape(-1, 1)

plt.scatter(range(len(X_train)), y_train, color='red')
plt.plot(range(len(X_train)), regressor.predict(X_train), color='blue')
plt.show()
