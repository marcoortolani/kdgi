from sklearn import svm

class Tree:
	def __init__(self, left, right, svm):
		self.left = left
		self.right = right
		self.svm = svm
		
	def add_node(self, x, svr, value):
		node, direction = self.branch_down(x)
		if(isinstance(node, Tree)):
			node.add_node(x, svr, value)
		else:
			if(isinstance(node, int)):
				if direction == "left":
					newnode = Tree(self.left, value, svr)
					self.left = newnode
					
				if direction == "right":
					newnode = Tree(self.right, value, svr)
					self.right = newnode
			else:
				raise ValueError('In function add_node of class Tree: parameter lr must be either "left" or "right".')
		
	def branch_down(self, x):
		y_pred = self.svm.predict([x])[0]
		if(y_pred > 0.5):
			return self.right, "right"
		else:
			return self.left, "left"
			
	def get_leaf(self, x):
		node, _ = self.branch_down(x)
		if(isinstance(node, int)):
			return node
		else:
			if(isinstance(node, Tree)):
				return node.get_leaf(x)
			else:
				raise ValueError('In function get_leaf of class Tree: got a non int and non Tree node.')
				
	def stamp(self, depth):
		s = ""
		for i in range(depth):
			s = s + "\t"
		print(s + "node")
		
		node = self.left
		if(isinstance(node, Tree)):
			node.stamp(depth + 1)
		else:
			print(s + "\t" + str(node))
		node = self.right
		if(isinstance(node, Tree)):
			node.stamp(depth + 1)
		else:
			print(s + "\t" + str(node))

class SVMClassifier:
	def __init__(self, X):
		if(len(X) < 2):
			raise ValueError('In function __init__ of class SVMClassifier: param X must be a list of at least 2 elements.')
		self.vectors = X[0:2]
		
		clf = svm.SVC(gamma='scale')
		clf.fit(X[0:2], [0, 1])
		self.root = Tree(0, 1, clf)
		
		self.add_elements(X[2:])
	
	def add_single_element(self, x1):
		index = self.root.get_leaf(x1)
		
		l = len(self.vectors)
		if(index >= l):
			raise ValueError('In function add_single_element of class SVMClassifier: got an index equal to or greater than num_elements.')
		
		x0 = self.vectors[index]
		
		clf = svm.SVC(gamma='scale')
		clf.fit([x0, x1], [0, 1])
		
		self.root.add_node(x1, clf, l)
		
		self.vectors = self.vectors + [x1]
	
	def add_elements(self, X):
		for x in X :
			self.add_single_element(x)
	
	def get_index(self, x):
		return self.root.get_leaf(x)
	
	def get_dim(self):
		return len(self.vectors)
	
	def stamp(self):
		i = 0
		for v in self.vectors :
			print(str(i) + str(v))
			i = i + 1
		self.root.stamp(0)

"""	
x0 = [0.0, 1.0, 0.6, 0.2, 0.84]
x1 = [1.0, 0.62, 0.84, 0.11, 0.0]
x2 = [0.5, 0.3, 0.76, 1, 0.4]
Dset = [x0, x1, x2]

C = SVMClassifier(Dset)

for x in Dset :
	print(C.root.get_leaf(x))

print()

C.stamp()
"""


"""
X = [x0, x1]
y = [0, 1]

clf = svm.SVC(gamma='scale')
clf.fit(X, y)

T = Tree(0, 1, clf)

X = [Dset[T.get_leaf(x2)], x2]
y = [0, 1]

clf = svm.SVC(gamma='scale')
clf.fit(X, y)
T.add_node(x2, clf, 2)

print(T.get_leaf(x0))
print(T.get_leaf(x1))
print(T.get_leaf(x2))
"""

"""
print(T)
print(T.left)
print(T.right)
print(T.right.left)
print(T.right.right)
"""
