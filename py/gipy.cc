#include <pybind11/pybind11.h>

#include "dfa.h"

namespace py = pybind11;

int add(int i, int j) {
    return i + j;
}

PYBIND11_MODULE(gi_gipy, m) {
    m.doc() = "pybind11 plugin for the kdgi library"; // module docstring

    m.def("addxxx", &add, "A function which adds two numbers",
          py::arg("i"), py::arg("j"));

    py::class_<Dfa>(m, "DFA")
      .def(py::init());
}
