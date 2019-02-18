import alutils as ut
import numpy as np
from keras.preprocessing import sequence
from keras import backend as K

class RNN:
	model = None
	#class_layer = None
	sequence_length = None
	
	def __init__(self, modelname):
		self.model = ut.get_model(modelname)
		#self.class_layer = layer
		inp = self.model.input
		self.sequence_length = inp.shape[1].value
		
	def membership_query(self, x):
		x = sequence.pad_sequences([x], maxlen=self.sequence_length)
		y = self.model.predict(x)
		return y[0][0]
	
	def get_layer_output(self, layer, x):
		#listofint = stringofint.split(" ")
		
		x = sequence.pad_sequences([x], maxlen=self.sequence_length)

		inp = self.model.input                    # input placeholder
		outputs = [self.model.layers[2].output]          # all layer outputs
		functor = K.function([inp, K.learning_phase()], outputs )
		outs = functor([x, 1.])
		
		#print(outs)
		
		return outs[0][0]
