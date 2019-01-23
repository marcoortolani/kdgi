import tensorflow as tf
import numpy as np

X0 = np.zeros([157, 128])
Y0 = np.zeros([157], dtype=np.int32)
X1 = np.ones([157, 128])
Y1 = np.ones([157], dtype=np.int32)
example_id = np.array(['%d' % i for i in range(len(Y0))])

x_column_name = 'x'
example_id_column_name = 'example_id'

train_input_fn0 = tf.estimator.inputs.numpy_input_fn(
    x={x_column_name: X0, example_id_column_name: example_id},
    y=Y0,
    num_epochs=None,
    shuffle=True)
    
train_input_fn1 = tf.estimator.inputs.numpy_input_fn(
    x={x_column_name: X1, example_id_column_name: example_id},
    y=Y1,
    num_epochs=None,
    shuffle=True)

svm = tf.contrib.learn.SVM(
    example_id_column=example_id_column_name,
    feature_columns=(tf.contrib.layers.real_valued_column(
        column_name=x_column_name, dimension=128),),
    l2_regularization=0.1)

svm.fit(input_fn=train_input_fn0, steps=10)
svm.fit(input_fn=train_input_fn1, steps=10)

#svm.evaluate(input_fn=train_input_fn0)
x = np.ones([2, 157]) * 0.2
print(x)
print(svm.predict(x))
