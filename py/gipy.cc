#include <pybind11/pybind11.h>

//For automatic conversions
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

//For operators overload: https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading
#include <pybind11/operators.h>

#include "concretedfa.h"

//Default arguments: https://pybind11.readthedocs.io/en/stable/basics.html#default-args

namespace py = pybind11;

// tenere d'occhio questo vector
using abstr = Dfa<std::__1::vector<DfaState, std::__1::allocator<DfaState> >*>;

PYBIND11_MODULE(gi_gipy, m) {
    m.doc() = "pybind11 plugin for the kdgi library";

    py::class_<abstr>(m, "AbstractDfa");

    py::class_<ConcreteDfa, abstr>(m, "Dfa")

      //*** Constructors ***
      .def(py::init(),"Make an instance of null dfa.")

      .def(py::init<const ConcreteDfa&>(),"Constructor for make a copy of a dfa \"d1\".")

      //===========================================================================================

      //*** Operators overload ***
      .def(py::self == py::self, "Definition of equivalence operator==.")

      .def(py::self < py::self, "Definition of minor operator<. Return true if calling DFA has less states than DFA @p d1.")

      //===========================================================================================

      //*** Basic methods ***
      .def("get_dim_alphabet", &ConcreteDfa::get_dim_alphabet,"Return size of alphabet.")

      .def("get_alphabet",&ConcreteDfa::get_alphabet,"Return a vector with alphabet symbols.")

      .def("get_accepting_states",&ConcreteDfa::get_accepting_states,"Return accepting states.")

      .def("is_accepting",&ConcreteDfa::is_accepting,"Returns true if current state is accepting.")

      .def("get_num_states",&ConcreteDfa::get_num_states,"Get number of states.")

      .def("get_start_state",&ConcreteDfa::get_start_state,"Get index of start state.")

      //.def("print_dfa_ttable",&ConcreteDfa::print_dfa_ttable,"Print the transition table of current dfa. Before the transition table print the title passed as parameter.")
      
      .def_static("read_dfa_file", &ConcreteDfa::read_dfa_file, "Read a dfa from a file.")
      
      .def("save_dfa", &ConcreteDfa::print_dfa_in_text_file, "Print a DFA in a text file. Adopted format is the same used for reading a DFA from file.")
      
      .def("print_dfa_dot", &ConcreteDfa::print_dfa_dot, "Print a dot file for the current dfa.")
      
      .def("membership_query", &ConcreteDfa::membership_query, "Make a membership query to dfa with the \"phrase\" string with alphabet symbol. Return \"true\" if the arrive state for \"phrase\" is acceptor, else \"false\".")
      
      .def("gen_samples", &ConcreteDfa::generate_pos_neg_samples_without_weights, "It returns a set random samples (all different) generated from current DFA.")
      
      //===========================================================================================

      //*** W-Method ***
      .def("get_w_method_test_set", &ConcreteDfa::get_w_method_test_set, "Return a W-METHOD test set of strings for current DFA.",py::arg("target_dfa"), py::arg("sigma") = 1)
      
      .def("get_w_method_statistics", &ConcreteDfa::get_w_method_statistics, "Return a 9-dimensional array with all the w-method statistics.")
      
      .def("print_w_method", &ConcreteDfa::print_w_method, "Print the w-method statistics")

      //===========================================================================================

      //*** Structural similarity ***
      .def("neighbour_matching", &ConcreteDfa::neighbour_matching, "Gives the structural similarity score matrix between every pair of states of two DFAs",py::arg("target_dfa"), py::arg("isomorphism") = 0, py::arg("color") = 0, py::arg("eps") = 0.0001)

      .def("print_structural_similarity", &ConcreteDfa::print_structural_similarity, "Print the matrix containing the similarity score between pair of nodes.")
      
      .def("struct_sim", &ConcreteDfa::struct_sim, "Return raw structural similarity score between two dfas (utility function)",py::arg("target_dfa"), py::arg("isomorphism") = 0, py::arg("color") = 0, py::arg("eps") = 0.0001)
      
      //===========================================================================================
      
      //*** Overall similarity ***
      .def("dfa_similarity", &ConcreteDfa::dfa_similarity, "Returns and print the similarity score between dfas, taking into account both linguistical and structural sides.",py::arg("target_dfa"),py::arg("print") = 0, py::arg("sigma") = 1, py::arg("isomorphism") = 0, py::arg("color") = 0, py::arg("eps") = 0.0001)
    ;


    py::class_<DfaSim>(m,"DfaSim")

      .def(py::init())

      //.def("print_sim", &DfaSim::print_sim, "Print the similarity score between dfas, taking into account both linguistical and structural sides.")

      .def("get_true_positives", &DfaSim::get_true_positives)
      .def("get_true_negatives", &DfaSim::get_true_negatives)
      .def("get_false_positives", &DfaSim::get_false_positives)
      .def("get_false_negatives", &DfaSim::get_false_negatives)
      .def("get_precision", &DfaSim::get_precision)
      .def("get_recall", &DfaSim::get_recall)
      .def("get_linguistical_f_measure", &DfaSim::get_linguistical_f_measure)
      .def("get_specificity", &DfaSim::get_specificity)
      .def("get_bcr", &DfaSim::get_bcr)
      .def("get_nodes_sim_matrix", &DfaSim::get_nodes_sim_matrix)
      .def("get_structural_f_measure", &DfaSim::get_structural_f_measure)
      .def("get_exec_time", &DfaSim::get_exec_time)

      //.def("which_dfas", (void (DfaSim::*)()const)  &DfaSim::which_dfas,"Print the ttable of reference and subject dfas")
      .def("which_dfas", (const ConcreteDfa* (DfaSim::*)(const ConcreteDfa*)const) &DfaSim::which_dfas,"Returns the two dfas compared")
    ;

    
}
