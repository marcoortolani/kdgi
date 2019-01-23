import pandas as pd
from collections import Counter
import pyprind
import string
import numpy as np

from keras.datasets import imdb
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers.embeddings import Embedding
from keras.preprocessing import sequence
from keras.models import  model_from_json

max_length = 20


def csv2intlist(X_label, y_label, filename, testTrainRatio=0.5, alphabet=['', 'a', 'b']):
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


#Definisce la classe OneHot, ovvero il livello di one hot endcoding, precedente al livello LSTM.
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

def build_and_train_model(filename, modelname, epochs, batch_size): #oracle6.cvs, model6, epochs=4000, batch_size=128
	num_words = 3
	X_train, y_train, X_test, y_test = csv2intlist('review', 'sentiment', filename="csv/" + filename + ".csv", testTrainRatio=0.75)
	# truncate and pad input sequences
	X_train = sequence.pad_sequences(X_train, maxlen=max_length)
	X_test = sequence.pad_sequences(X_test, maxlen=max_length)

	# create the model
	model = Sequential()
	model.add(OneHot(input_dim=num_words, input_length=max_length))
	model.add(LSTM(10, activation='sigmoid'))
	model.add(Dense(5, activation='relu'))
	model.add(Dense(1, activation='sigmoid'))

	model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])

	print(model.summary())

	model.fit(X_train, y_train, validation_data=(X_test, y_test), epochs=epochs, batch_size=batch_size)
	#model.fit(X_train, y_train, validation_split=0.25, epochs=600, batch_size=128)

	# Final evaluation of the model
	scores = model.evaluate(X_test, y_test, verbose=0)
	autoscores = model.evaluate(X_train, y_train, verbose=0)
	print("Test Accuracy: %.2f%%" % (scores[1]*100))
	print("Train Accuracy: %.2f%%" % (autoscores[1]*100))

	json_string = model.to_json()
	f = open("models/" + modelname + ".txt", "a")
	f.write(json_string) 

	model.save_weights("models/" + modelname + ".hdf5")

def get_model(modelname):
	f = open("models/" + modelname + ".txt", "r")
	json_string = f.readline()
	model = model_from_json(json_string)
	model.load_weights("models/" + modelname + ".hdf5")
	model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
	print(model.summary())

	return model
	
def get_layer_output(model, layer, x):
	x = sequence.pad_sequences(x, maxlen=max_length)

	from keras import backend as K

	inp = model.input                    # input placeholder
	outputs = [model.layers[2].output]          # all layer outputs
	functor = K.function([inp, K.learning_phase()], outputs )
	outs = functor([x, 1.])

	return outs
