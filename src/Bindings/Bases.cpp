// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* PyBind11 */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

/* Internal */
#include "Definition.h"
#include "ConformRule.h"

VOID_NAMESPACE_OPEN

namespace py = pybind11;

namespace bindings {

class PyConformRule : public ConformRule
{
public:
    using ConformRule::ConformRule;

    std::string Name() const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(std::string, ConformRule, "name", Name);
    }

    bool Match(
        const std::unordered_map<std::string, std::string>& sourceMetadata,
        const std::unordered_map<std::string, std::string>& candidateMetadata
    ) const override
    {
        PYBIND11_OVERRIDE_PURE_NAME(bool, ConformRule, "match", Match, sourceMetadata, candidateMetadata);
    }
};

void BindBases(py::module_& m)
{
    m.doc() = "Void Bases Module.";
    
    py::class_<ConformRule, PyConformRule>(m, "ConformRule")
        .def(py::init<>())
        .def("name", &ConformRule::Name)
        .def("match", &ConformRule::Match);
}

} // namespace bindings

VOID_NAMESPACE_CLOSE
