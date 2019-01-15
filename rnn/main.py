import net
## Preprocessing the data:
## Separate words and
## count each word's occurrence>>>
import pyprind
import pandas as pd
import string
import re
from collections import Counter

df = pd.read_csv('oracle.csv', encoding='utf-8')

counts = Counter()
pbar = pyprind.ProgBar(len(df['review']), title='Counting words occurrences')
for i,review in enumerate(df['review']):
	text = ''.join([c if c not in string.punctuation else ' '+c+' ' for c in review]).lower()
	df.loc[i,'review'] = text
	pbar.update()
	counts.update(text.split())
## Create a mapping
## Map each unique word to an integer
word_counts = sorted(counts, key=counts.get, reverse=True)
print(word_counts[:5])
word_to_int = {word: ii for ii, word in enumerate(word_counts, 1)}
mapped_reviews = []
pbar = pyprind.ProgBar(len(df['review']), title='Map reviews to ints')
for review in df['review']:
	mapped_reviews.append([word_to_int[word] for word in review.split()])
	pbar.update()
n_words = max(list(word_to_int.values())) + 1




## Define same-length sequences
## if sequence length < 200: left-pad with zeros
## if sequence length > 200: use the last 200 elements
sequence_length = 50
## (Known as T in our RNN formulas)>>> 
sequences = np.zeros((len(mapped_reviews), sequence_length), dtype=int)
for i, row in enumerate(mapped_reviews):
	review_arr = np.array(row)
	sequences[i, -len(row):] = review_arr[-sequence_length:]

l = int(len(sequences) / 2)
X_train = sequences[:l,:]
y_train = df.loc[:l, 'sentiment'].values
X_test = sequences[l:,:]
y_test = df.loc[l:, 'sentiment'].values

rnn = net.SentimentRNN(n_words=n_words, seq_len=sequence_length, embed_size=32, lstm_size=64, num_layers=1, batch_size=10, learning_rate=0.001)
rnn.train(X_train, y_train, num_epochs=40)

preds = rnn.predict(X_test)
y_true = y_test[:len(preds)]
print('Test Acc.: %.3f' % (np.sum(preds == y_true) / len(y_true)))
