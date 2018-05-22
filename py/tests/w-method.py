from gi_gipy import *

dfa_file1 = '../data/tomita1.txt'
dfa_file2 = '../data/tomita2.txt'

dfa1 = Dfa.read_dfa_file(dfa_file1)
dfa2 = Dfa.read_dfa_file(dfa_file2)

testset = dfa1.get_w_method_test_set(dfa2)
#print (testset)

dfa1.print_w_method(dfa1.get_w_method_statistics(testset,dfa2))

# TP = 0
# FN = 0
# TN = 0
# FP = 0
# for s in testset:
#     q1 = dfa1.membership_query(s)
#     q2 = dfa2.membership_query(s)
#     if q1:
#         if q2:
#             TP += 1
#         else:
#             FN += 1
#     else:
#         if q2:
#             FP += 1
#         else:
#             TN += 1

# print [TP, FN, TN, FP]

