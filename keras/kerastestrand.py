import numpy as np
from keras.datasets import imdb
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers.embeddings import Embedding
from keras.preprocessing import sequence
# fix random seed for reproducibility
np.random.seed(123)

def get_random_data(n_words, length, n_train, n_test):
	X_train = []
	y_train = []
	X_test = []
	y_test = []
	for i in range(n_train):
		X_train = X_train + [[int(n_words*np.random.random()) for i in range(length)]]
		y_train = y_train + [round(np.random.random())]
	
	for i in range(n_test):
		X_test = X_test + [[int(n_words*np.random.random()) for i in range(length)]]
		y_test = y_test + [round(np.random.random())]
	
	return X_train, y_train, X_test, y_test
	

top_words = 50
max_len = 15

X_train, y_train, X_test, y_test = getdata.csv2intlist('review', 'sentiment', filename='oracle.csv', testTrainRatio=0.75)

#X_train, y_train, X_test, y_test = get_random_data(top_words, 10, 200, 20)

# truncate and pad input sequences
max_review_length = 20
X_train = sequence.pad_sequences(X_train, maxlen=max_review_length)
X_test = sequence.pad_sequences(X_test, maxlen=max_review_length)

"""
print('X_train')
for i in range(min(len(X_train), 10)):
	print(X_train[i])
print('X_test')
for i in range(min(len(X_test), 10)):
	print(X_test[i])
"""

# create the model
embedding_vecor_length = 32
#embedding_vecor_length = 2
model = Sequential()
model.add(Embedding(top_words, embedding_vecor_length, input_length=max_review_length))
model.add(LSTM(100))
model.add(Dense(1, activation='sigmoid'))

model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])

print(model.summary())
model.fit(X_train, y_train, validation_data=(X_train, y_train), epochs=3, batch_size=128)

# Final evaluation of the model
scores = model.evaluate(X_train, y_train, verbose=0)
print("Accuracy: %.2f%%" % (scores[1]*100))
