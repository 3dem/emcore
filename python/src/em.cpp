#include <string>
#include <pybind11/pybind11.h>
#include "em/base/array.h"

namespace py = pybind11;

using namespace em;


// Define each submodule separately
void init_submodule_base(py::module &);
void init_submodule_image(py::module &);


PYBIND11_MODULE(em, m) {
    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: em

        .. autosummary::
           :toctree: _generate

           add
           subtract
    )pbdoc";

    init_submodule_base(m);
    init_submodule_image(m);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}