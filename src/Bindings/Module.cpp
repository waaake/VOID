// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Pybind11 */
#include <pybind11/pybind11.h>

/* Internal */
#include "Definition.h"

namespace py = pybind11;

/* Forward decl */
VOID_NAMESPACE_OPEN

namespace bindings {

    void BindCore(py::module_&);
    void BindUi(py::module_&);

} // namespace bindings

PYBIND11_MODULE(voidpy, m)
{
    m.doc() = "Void Python Binding";

    py::module_ core = m.def_submodule("core", "Void Core Module");
    py::module_ ui = m.def_submodule("ui", "Void UI Module");
    VOID_NAMESPACE::bindings::BindCore(core);
    VOID_NAMESPACE::bindings::BindUi(ui);
}

VOID_NAMESPACE_CLOSE
