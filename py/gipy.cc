#include <pybind11/pybind11.h>

//For automatic conversions
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

#include <pybind11/stl_bind.h>

//For operators overload: https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading
#include <pybind11/operators.h>

#include "concretedfa.h"
#include "angluindfa.h"
#include "angluinlearner.h"
#include "tttdfa.h"
#include "tttlearner.h"

//Default arguments: https://pybind11.readthedocs.io/en/stable/basics.html#default-args

namespace py = pybind11;

//Definiamo PyDfa come trampolino della classe Dfa per poter gestire i suoi metodi virtuali.
//Deve essere anche generica in quanto Dfa è generica.
template <class Iter>
class PyDfa : public Dfa<Iter> {
public:

	/* Aliases for the begin and end methods*/
	using 	basetype = typename std::iterator_traits< Iter >::value_type;
	using 	iterator = typename basetype::iterator;

    /* Inherit the constructors */
    using Dfa<Iter>::Dfa;

    /* Trampoline (need one for each virtual function) */
    bool membership_query(vector<symbol_> phrase) const override {
        PYBIND11_OVERLOAD_PURE(
            bool, /* Return type */
            Dfa<Iter>,      /* Parent class */
            membership_query,	/* Name of function in C++ (must match Python name) */
            phrase      /* Argument(s) */
        );
    }
    
    /* Trampoline (need one for each virtual function) */
    iterator begin() override {
        PYBIND11_OVERLOAD_PURE(
            iterator, /* Return type */
            Dfa<Iter>,      /* Parent class */
            begin	/* Name of function in C++ (must match Python name) */
                  /* Argument(s) */
        );
    }
    
    /* Trampoline (need one for each virtual function) */
    iterator end() override {
        PYBIND11_OVERLOAD_PURE(
            iterator, /* Return type */
            Dfa<Iter>,      /* Parent class */
            end	/* Name of function in C++ (must match Python name) */
                  /* Argument(s) */
        );
    }
};

//templated declaration for Dfa<Iter>
//Questo funzione generica viene usata per istanziare le classi. 
//In pratica il codice viene riscritto ogni volta che la funzione viene richiamata,
//ma in questo modo lo scriviamo una sola volta nei sorgenti.
template<typename Iter>
void declare_Dfa(py::module &m, std::string typestr) {
	
	std::string pyclass_name = std::string("Dfa") + typestr;
	std::cout << pyclass_name.c_str() << endl;
	
	//Dfa<Iter> non può essere instanziata normalmente, perchè oltre ad essere una classe generica è anche astratta.
	//Bisogna passare per una classe "trampolino" definita (sopra) solo in python proprio per questo scopo,
	//la quale gestirà le funzioni (puramente e non) virtuali. Questa classe è solo un trampolino, per cui tutti i
	//binding dei metodi saranno riferiti alla classe reale.
	py::class_<Dfa<Iter>, PyDfa<Iter>>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		/* Constructor */
		.def(py::init<>())
		
		/* Common methods */
		.def("get_dim_alphabet", &Dfa<Iter>::get_dim_alphabet)
		.def("get_alphabet", &Dfa<Iter>::get_alphabet)
		.def("print_dfa", &Dfa<Iter>::print)
		.def("print_dot", &Dfa<Iter>::print_dfa_dot)
		.def("print_structural_similarity", &Dfa<Iter>::print_structural_similarity)
		
		/* Pure virtual functions */
		.def("membership_query", &Dfa<Iter>::membership_query)
		.def("begin", &Dfa<Iter>::begin)
		.def("end", &Dfa<Iter>::end)
	;
}

template<typename D1, typename D2>
std::pair<bool, vector<symbol_>> equivalence_query(D1* d1, D2* d2){
	vector<symbol_> counterexample;
	bool res = d1->equivalence_query(d2, counterexample);
	return std::make_pair(res, counterexample);
};

template<typename D1, typename D2>
vector< vector< double > > neighbour_matching(D1* d1, D2* d2){//, bool isomorphism=false, bool color=false, double eps=0.0001){
	return d1->neighbour_matching(d2);//, isomorphism, color, eps);
};

template<typename Learner, typename Oracle>
void declare_Learner(py::module &m, std::string typestr) {
    using Class = Learner;
    std::string pyclass_name = typestr;
    std::cout << pyclass_name.c_str() << endl;
    
    py::class_<Class>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<Oracle*, vector<symbol_>>())
		.def("learn", &Learner::learn)
    ;
}

//Ho cambiato il nome del modulo per farlo matchare con il target
//PYBIND11_MODULE(gi_gipy, m) {
PYBIND11_MODULE(gipy_lib, m) {
    m.doc() = "pybind11 plugin for the kdgi library";
	
	//Instanziamo il Dfa<vector...> prima di poterlo usare come classe base per il ConcreteDfa
	declare_Dfa<vector<DfaState>*>(m, "_vec");
	
	//A questo punto possiamo definire il ConcreteDfa come classe derivata di Dfa<vector...>
	py::class_<ConcreteDfa, Dfa<vector<DfaState>*>>(m, "ConcreteDfa")

      //*** Constructors ***
      .def(py::init(),"Make an instance of null dfa.")

      .def(py::init<const ConcreteDfa&>(),"Constructor for make a copy of a dfa \"d1\".")

      //===========================================================================================

      //*** Operators overload ***
      .def(py::self == py::self, "Definition of equivalence operator==.")

      .def(py::self < py::self, "Definition of minor operator<. Return true if calling DFA has less states than DFA @p d1.")

      //===========================================================================================

      //*** Basic methods ***
      .def_static("read_dfa_file", &ConcreteDfa::read_dfa_file)
      .def("print_dfa_ttable",&ConcreteDfa::print_dfa_ttable,"Print the transition table of current dfa. Before the transition table print the title passed as parameter.")
      
    ;

	/* Active learning */

	/*----------Angluin----------*/
	// AngluinDfa eredita da Dfa<vector...>
    py::class_<AngluinDfa, Dfa<vector<DfaState>*>>(m, "AngluinDfa");
    
    /* dichiaro i learner angluin per i tipi di oracoli che mi interessano 
     * (_C per ConcreteDfa e _A per AngluinDfa) 
     */
    declare_Learner<AngluinLearner<ConcreteDfa>, ConcreteDfa>(m, "AngluinLearner_C");
    declare_Learner<AngluinLearner<AngluinDfa>, AngluinDfa>(m, "AngluinLearner_A");
    declare_Learner<AngluinLearner<TTTDfa>, TTTDfa>(m, "AngluinLearner_T");
    
    /*----------TTT----------*/
    // TTT eredita da Dfa<list...>
    declare_Dfa<list<DfaState>*>(m, "_list");
    py::class_<TTTDfa, Dfa<list<DfaState>*>>(m, "TTTDfa");
    
    declare_Learner<TTTLearner<ConcreteDfa>, ConcreteDfa>(m, "TTTLearner_C");
    declare_Learner<TTTLearner<AngluinDfa>, AngluinDfa>(m, "TTTLearner_A");
    declare_Learner<TTTLearner<TTTDfa>, TTTDfa>(m, "TTTLearner_T");
    
    /*----------Function templates----------*/
    //Equivalence query
    m.def("equivalence_query", equivalence_query<ConcreteDfa, ConcreteDfa>);
    m.def("equivalence_query", equivalence_query<ConcreteDfa, AngluinDfa>);
    m.def("equivalence_query", equivalence_query<ConcreteDfa, TTTDfa>);
    
    m.def("equivalence_query", equivalence_query<AngluinDfa, AngluinDfa>);
    m.def("equivalence_query", equivalence_query<AngluinDfa, ConcreteDfa>);
    m.def("equivalence_query", equivalence_query<AngluinDfa, TTTDfa>);
    
    m.def("equivalence_query", equivalence_query<TTTDfa, TTTDfa>);
    m.def("equivalence_query", equivalence_query<TTTDfa, ConcreteDfa>);
    m.def("equivalence_query", equivalence_query<TTTDfa, AngluinDfa>);
    
    //Neighbour matching
    m.def("neighbour_matching", neighbour_matching<ConcreteDfa, ConcreteDfa>);
    m.def("neighbour_matching", neighbour_matching<ConcreteDfa, AngluinDfa>);
    m.def("neighbour_matching", neighbour_matching<ConcreteDfa, TTTDfa>);
    
    m.def("neighbour_matching", neighbour_matching<AngluinDfa, AngluinDfa>);
    m.def("neighbour_matching", neighbour_matching<AngluinDfa, ConcreteDfa>);
    m.def("neighbour_matching", neighbour_matching<AngluinDfa, TTTDfa>);
    
    m.def("neighbour_matching", neighbour_matching<TTTDfa, TTTDfa>);
    m.def("neighbour_matching", neighbour_matching<TTTDfa, ConcreteDfa>);
    m.def("neighbour_matching", neighbour_matching<TTTDfa, AngluinDfa>);
}
