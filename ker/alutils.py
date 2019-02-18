import pandas as pd
from collections import Counter
import pyprind
import string
import numpy as np

from keras.layers.embeddings import Embedding
from keras.preprocessing import sequence
from keras.models import  model_from_json

#max_length = 20


def csv2intlist(X_label, y_label, filename, testTrainRatio, alphabet):
	#df = pd.read_csv(filename, encoding='utf-8').sample(frac=1, replace=True).reset_index(drop=True)
	df = pd.read_csv(filename, encoding='utf-8').sample(frac=1.0, replace=False).reset_index(drop=True)
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
	
def train_model(model, filename, modelname, epochs, batch_size, testTrainRatio, sequence_length, validate, alphabet): #"oracle6", "model6", epochs=4000, batch_size=128
	num_words = len(alphabet)
	X_train, y_train, X_test, y_test = csv2intlist('word', 'acceptance', filename="csv/" + filename + ".csv", testTrainRatio=testTrainRatio, alphabet=alphabet)
	print("X_train")
	for el in  X_train:
		print(el)
	print
	print("X_test")
	for el in  X_test:
		print(el)
	# truncate and pad input sequences
	X_train = sequence.pad_sequences(X_train, maxlen=sequence_length)
	X_test = sequence.pad_sequences(X_test, maxlen=sequence_length)
	
	if(validate):
		model.fit(X_train, y_train, validation_data=(X_test, y_test), epochs=epochs, batch_size=batch_size)
	else:
		model.fit(X_train, y_train, validation_data=(X_train, y_train), epochs=epochs, batch_size=batch_size)

	json_string = model.to_json()
	f = open("models/" + modelname + ".txt", "a")
	f.write(json_string) 

	model.save_weights("models/" + modelname + ".hdf5")

	# Final evaluation of the model
	autoscores = model.evaluate(X_train, y_train, verbose=0)
	print("Train Accuracy: %.2f%%" % (autoscores[1]*100))
	
	if(validate):
		scores = model.evaluate(X_test, y_test, verbose=0)
		print("Test Accuracy: %.2f%%" % (scores[1]*100))

def get_model(modelname):
	f = open("models/" + modelname + ".txt", "r")
	json_string = f.readline()
	model = model_from_json(json_string)
	model.load_weights("models/" + modelname + ".hdf5")
	model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])
	print(model.summary())

	return model

"""
def get_layer_output(model, layer, x):
	x = sequence.pad_sequences(x, maxlen=max_length)

	from keras import backend as K

	inp = model.input                    # input placeholder
	outputs = [model.layers[2].output]          # all layer outputs
	functor = K.function([inp, K.learning_phase()], outputs )
	outs = functor([x, 1.])

	return outs
"""
