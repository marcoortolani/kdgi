import alutils as ut
import numpy as np
from keras.preprocessing import sequence

class LSTMOracle:
	model = None
	class_layer = None
	sequence_length = None
	
	def __init__(self, modelname, layer):
		self.model = ut.get_model(modelname)
		self.class_layer = layer
		inp = self.model.input
		self.sequence_length = inp.shape[1].value
		
	def membership_query(self, stringofint):
		listofint = stringofint.split(" ")
		print(listofint)
		X = sequence.pad_sequences([listofint], maxlen=self.sequence_length)
		y = self.model.predict(X)
		return y[0][0]
