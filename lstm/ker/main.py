from gipy_lib import *
import LSTMOracle
import alutils as ut
import numpy as np
from keras.preprocessing import sequence
"""
filename="oracle6"
modelname="model6"
ut.build_and_train_model(filename, modelname, 4000, 128)

model = ut.get_model(modelname)

x = [[1, 1, 1, 2, 2, 1, 2]]

print(ut.get_layer_output(model, 2, x))
"""

#ut.build_and_train_model("oracle6", "test", 20, 70)

o = LSTMOracle.LSTMOracle("model6", 2)
o.membership_query([1, 1, 1, 2, 1])

l = TTTLearner_S(o, ["a", "b"])
