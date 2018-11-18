from gipy_lib import ConcreteDfa

from gipy_lib import AngluinDfa
from gipy_lib import AngluinLearner_A
from gipy_lib import AngluinLearner_C
from gipy_lib import AngluinLearner_T

from gipy_lib import TTTDfa
from gipy_lib import TTTLearner_A
from gipy_lib import TTTLearner_C
from gipy_lib import TTTLearner_T

from gipy_lib import equivalence_query
from gipy_lib import neighbour_matching

c = ConcreteDfa.read_dfa_file("data/_all_except_abab.txt")
c.print_dot("not_abab_c", "dfas/dot/not_abab_c.dot")

lc = AngluinLearner_C(c, c.get_alphabet())
ac = lc.learn()
ac.print_dot("not_abab_ac", "dfas/dot/not_abab_ac.dot")
#cntr = equivalence_query(c, ac)
#print("Equivalence query tra un concrete dfa e un angluin dfa (stesso linguaggio)")
#print(cntr)

c = ConcreteDfa.read_dfa_file("data/_odd_a_odd_b.txt")
c.print_dot("odd_a_odd_b_c", "dfas/dot/odd_a_odd_b_c.dot")

tttl = TTTLearner_C(c, c.get_alphabet())
ttt = tttl.learn()
c.print_dot("odd_a_odd_b_ttt", "dfas/dot/odd_a_odd_b_ttt.dot")

#cntr = equivalence_query(c, ttt)
#print("Equivalence query tra un concrete dfa e un ttt dfa (stesso linguaggio)")
#print(cntr)

cntr = equivalence_query(ac, ttt)
print("Equivalence query tra un ttt dfa e un angluin dfa (qualunque stringa meno abab e qualunque stringa)")
print(cntr)

n = neighbour_matching(ac, ttt)
print("Neighbour matching:")
print(n)
