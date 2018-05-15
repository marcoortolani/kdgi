#include <pybind11/pybind11.h>

#include "dfa.h"

namespace py = pybind11;

PYBIND11_MODULE(gi_gipy, m) {
    py::class_<Dfa>(m, "DFA")
      .def(py::init());
}