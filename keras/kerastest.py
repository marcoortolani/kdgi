import numpy
from keras.datasets import imdb
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers.embeddings import Embedding
from keras.preprocessing import sequence
# fix random seed for reproducibility
numpy.random.seed(123)

import getdata

from keras.layers import Lambda
# We will use `one_hot` as implemented by one of the backends
from keras import backend as K

def OneHot(input_dim=None, input_length=None):
    # Check if inputs were supplied correctly
    if input_dim is None or input_length is None:
        raise TypeError("input_dim or input_length is not set")

    # Helper method (not inlined for clarity)
    def _one_hot(x, num_classes):
        return K.one_hot(K.cast(x, 'uint8'),
                          num_classes=num_classes)

    # Final layer representation as a Lambda layer
    return Lambda(_one_hot,
                  arguments={'num_classes': input_dim},
                  input_shape=(input_length,))


def test(filename):
	"""
	# load the dataset but only keep the top n words, zero the rest
	top_words = 5000
	(X_train, y_train), (X_test, y_test) = imdb.load_data(num_words=top_words)
	"""

	top_words = 5000
	X_train, y_train, X_test, y_test = getdata.csv2intlist('review', 'sentiment', filename=filename, testTrainRatio=0.75)

	# truncate and pad input sequences
	max_review_length = 20
	X_train = sequence.pad_sequences(X_train, maxlen=max_review_length)
	X_test = sequence.pad_sequences(X_test, maxlen=max_review_length)

	# create the model
	#embedding_vecor_length = 32
	embedding_vecor_length = 4
	model = Sequential()
	#model.add(Embedding(top_words, embedding_vecor_length, input_length=max_review_length))
	model.add(OneHot(input_dim=top_words, input_length=max_review_length))
	model.add(LSTM(40, activation='sigmoid'))
	model.add(Dense(20, activation='relu'))
	model.add(Dense(10, activation='relu'))
	model.add(Dense(1, activation='sigmoid'))

	model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])

	print(model.summary())

	#model.fit(X_train, y_train, validation_data=(X_test, y_test), epochs=1000, batch_size=128)
	model.fit(X_train, y_train, validation_split=0.25, epochs=1000, batch_size=128)

	# Final evaluation of the model
	scores = model.evaluate(X_test, y_test, verbose=0)
	#scores = model.evaluate(X_train, y_train, verbose=0)
	print("Accuracy: %.2f%%" % (scores[1]*100))

	#y_pred = [model.predict(X_test)]
	#print(y_pred)
