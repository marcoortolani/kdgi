#include <pybind11/pybind11.h>

//For automatic conversions
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

//For operators overload: https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading
#include <pybind11/operators.h>

#include "dfa.h"

//Default arguments: https://pybind11.readthedocs.io/en/stable/basics.html#default-args

namespace py = pybind11;

PYBIND11_MODULE(gi_gipy, m) {
    m.doc() = "pybind11 plugin for the kdgi library";

    py::class_<Dfa>(m, "Dfa")

      //*** Constructors ***
      .def(py::init(),"Make an instance of null dfa.")

      .def(py::init<const Dfa&>(),"Constructor for make a copy of a dfa \"d1\".")

      //===========================================================================================

      //*** Operators overload ***
      .def(py::self == py::self, "Definition of equivalence operator==.")

      .def(py::self < py::self, "Definition of minor operator<. Return true if calling DFA has less states than DFA @p d1.")

      //===========================================================================================

      //*** Basic methods ***
      .def("get_dim_alphabet", &Dfa::get_dim_alphabet,"Return size of alphabet.")

      .def("get_alphabet",&Dfa::get_alphabet,"Return a vector with alphabet symbols.")

      .def("get_accepting_states",&Dfa::get_accepting_states,"Return accepting states.")

      .def("is_accepting",&Dfa::is_accepting,"Returns true if current state is accepting.")

      .def("get_num_states",&Dfa::get_num_states,"Get number of states.")

      .def("get_start_state",&Dfa::get_start_state,"Get index of start state.")

      //.def("print_dfa_ttable",&Dfa::print_dfa_ttable,"Print the transition table of current dfa. Before the transition table print the title passed as parameter.")
      
      .def_static("read_dfa_file", &Dfa::read_dfa_file, "Read a dfa from a file.")
      
      .def("save_dfa", &Dfa::print_dfa_in_text_file, "Print a DFA in a text file. Adopted format is the same used for reading a DFA from file.")
      
      .def("print_dfa_dot", &Dfa::print_dfa_dot, "Print a dot file for the current dfa.")
      
      .def("membership_query", &Dfa::membership_query, "Make a membership query to dfa with the \"phrase\" string with alphabet symbol. Return \"true\" if the arrive state for \"phrase\" is acceptor, else \"false\".")
      
      .def("gen_samples", &Dfa::generate_pos_neg_samples_without_weights, "It returns a set random samples (all different) generated from current DFA.")
      
      //===========================================================================================

      //*** W-Method ***
      .def("get_w_method_test_set", &Dfa::get_w_method_test_set, "Return a W-METHOD test set of strings for current DFA.",py::arg("target_dfa"), py::arg("sigma") = 1)
      
      .def("get_w_method_statistics", &Dfa::get_w_method_statistics, "Return a 9-dimensional array with all the w-method statistics.")
      
      .def("print_w_method", &Dfa::print_w_method, "Print the w-method statistics")

      //===========================================================================================

      //*** Structural similarity ***
      .def("neighbour_matching_structural_similarity", &Dfa::neighbour_matching_structural_similarity, "Gives the structural similarity score matrix between every pair of states of two DFAs",py::arg("target_dfa"), py::arg("eps") = 0.0001, py::arg("color") = 0)
      
      .def("print_structural_similarity", &Dfa::print_structural_similarity, "Print the matrix containing the similarity score between pair of nodes.")
      
      //===========================================================================================
      
      .def("dfa_similarity", &Dfa::dfa_similarity, "Returns and print the similarity score between dfas, taking into account both linguistical and structural sides.",py::arg("target_dfa"),py::arg("print") = 0, py::arg("sigma") = 1, py::arg("eps") = 0.0001, py::arg("color") = 0)
      ;

}
