import alutils as al

from keras.datasets import imdb
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM

alphabet = ["", "a", "b"]
num_words = len(alphabet)
sequence_length = 15

# create the model
model = Sequential()
model.add(al.OneHot(input_dim=num_words, input_length=sequence_length))
model.add(LSTM(10, activation='sigmoid'))
model.add(Dense(5, activation='relu'))
model.add(Dense(1, activation='sigmoid'))

model.compile(loss='binary_crossentropy', optimizer='adam', metrics=['accuracy'])

print(model.summary())

# train and save the model
#al.train_model(model=model, filename="oracle4", modelname="model4c", epochs=10, batch_size=56, testTrainRatio=0.75, sequence_length=sequence_length, validate=True, alphabet=alphabet)
#al.train_model(model=model, filename="oracle6", modelname="model6b", epochs=4000, batch_size=128, testTrainRatio=1.0, sequence_length=sequence_length, validate=False, alphabet=alphabet)
al.train_model(model=model, filename="test", modelname="test", epochs=100, batch_size=128, testTrainRatio=1.0, sequence_length=sequence_length, validate=False, alphabet=alphabet)
