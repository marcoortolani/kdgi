//for python interpreter embedding
#include <pybind11/embed.h>
//for type conversion c++/python
#include<pybind11/stl.h>

#include <iostream>
#include <vector>
#include <string>

namespace py = pybind11;
using namespace py::literals;
using namespace std;

class LSTMOracle {
	private:
		py::object* net_;
		std::vector<string> alphabet_;
		
	public:
		LSTMOracle(std::string modelname, int layer, std::vector<std::string> alphabet, py::object* n);
		bool membership_query(std::vector<std::string> phrase);
};
