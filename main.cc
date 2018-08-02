#include <iostream>

#include "dfa.h"
#include "concretedfa.h"

int main() {
	ConcreteDfa d1, d2;
	d1 = d1.read_dfa_file("../py/data/automata/tomita15.txt");
	d2 = d2.read_dfa_file("../py/data/automata/tomita14.txt");

	d1.print_dfa_ttable("ciao");

	//d1.table_filling1(&d2);

	return 0;
}
