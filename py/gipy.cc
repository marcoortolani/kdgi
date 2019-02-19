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
#include "randomoracle.h"
#include "sillyoracle.h"

#ifdef PYEMBED
#include "rnnoracle.h"
#endif

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
		.def("get_num_states", &Dfa<Iter>::get_num_states)
		
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
py::class_<Learner> declare_Learner(py::module &m, std::string typestr) {
    //using Class = Learner;
    std::string pyclass_name = typestr;
    std::cout << pyclass_name.c_str() << endl;
    
    return py::class_<Learner>(m, pyclass_name.c_str(), py::buffer_protocol(), py::dynamic_attr())
		.def(py::init<Oracle*, vector<symbol_>>())
		.def("learn", &Learner::learn)
		.def("get_costs", &Learner::get_costs)
		.def("reset_costs", &Learner::reset_costs)
    ;
}

template<typename Oracle>
py::class_<TTTLearner<Oracle>> declare_TTTLearner(py::module &m, std::string templstr) {
	std::string pyclass_name = "TTTLearner" + templstr;
	
	auto LClass = declare_Learner<TTTLearner<Oracle>, Oracle>(m, pyclass_name);
	LClass.def("set_opack", &TTTLearner<Oracle>::set_opack);
	
	return LClass;
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
	
	//Learner
	
	/*----------Angluin----------*/
	// AngluinDfa eredita da Dfa<vector...>
    py::class_<AngluinDfa, Dfa<vector<DfaState>*>>(m, "AngluinDfa");
    
    /* dichiaro i learner angluin per i tipi di oracoli che mi interessano 
     * (_C per ConcreteDfa e _A per AngluinDfa etc) 
     */
    declare_Learner<AngluinLearner<ConcreteDfa>, ConcreteDfa>(m, "AngluinLearner_C");
    declare_Learner<AngluinLearner<AngluinDfa>, AngluinDfa>(m, "AngluinLearner_A");
    declare_Learner<AngluinLearner<TTTDfa>, TTTDfa>(m, "AngluinLearner_T");
    declare_Learner<AngluinLearner<SillyOracle>, SillyOracle>(m, "AngluinLearner_S");
    declare_Learner<AngluinLearner<RandomOracle>, RandomOracle>(m, "AngluinLearner_R");
    
	#ifdef PYEMBED
    declare_Learner<AngluinLearner<RNNOracle>, RNNOracle>(m, "AngluinLearner_RNN");
    #endif
    
    /*----------TTT----------*/
    // TTT eredita da Dfa<list...>
    declare_Dfa<list<DfaState>*>(m, "_list");
    py::class_<TTTDfa, Dfa<list<DfaState>*>>(m, "TTTDfa")
		.def("print_ttt", &TTTDfa::print_ttt_dot)
    ;
    
    declare_TTTLearner<ConcreteDfa>(m, "_C");
    declare_TTTLearner<AngluinDfa>(m, "_A");
    declare_TTTLearner<TTTDfa>(m, "_T");
    declare_TTTLearner<SillyOracle>(m, "_S");
    declare_TTTLearner<RandomOracle>(m, "_R");
    
    #ifdef PYEMBED
    declare_TTTLearner<RNNOracle>(m, "_RNN");
    #endif
    /*----------Oracles----------*/
    py::class_<RandomOracle>(m, "RandomOracle")
		.def(py::init<int, vector<symbol_>>())
		.def("membership_query",&RandomOracle::membership_query)
	;
	
    py::class_<SillyOracle>(m, "SillyOracle")
		.def(py::init< vector<vector<symbol_>> >())
		.def(py::init< vector<vector<symbol_>>, int >())
		.def("membership_query",&SillyOracle::membership_query)
	;
	
	#ifdef PYEMBED
	py::class_<RNNOracle>(m, "RNNOracle")
		.def(py::init< string, vector<symbol_>, int, int >())
		.def("membership_query",&RNNOracle::membership_query)
	;
	#endif
    
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
    
    m.def("equivalence_query", equivalence_query<RandomOracle, ConcreteDfa>);
    m.def("equivalence_query", equivalence_query<RandomOracle, AngluinDfa>);
    m.def("equivalence_query", equivalence_query<RandomOracle, TTTDfa>);
    
    m.def("equivalence_query", equivalence_query<SillyOracle, ConcreteDfa>);
    m.def("equivalence_query", equivalence_query<SillyOracle, AngluinDfa>);
    m.def("equivalence_query", equivalence_query<SillyOracle, TTTDfa>);
    
    #ifdef PYEMBED
    m.def("equivalence_query", equivalence_query<RNNOracle, ConcreteDfa>);
    m.def("equivalence_query", equivalence_query<RNNOracle, AngluinDfa>);
    m.def("equivalence_query", equivalence_query<RNNOracle, TTTDfa>);
	#endif
    
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
