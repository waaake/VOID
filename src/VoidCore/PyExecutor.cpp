// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "PyExecutor.h"

VOID_NAMESPACE_OPEN

namespace py = pybind11;

PyExecutor::PyExecutor()
    : m_Guard{}
    , m_Globals(py::globals())
{
    // py::initialize_interpreter();
}

PyExecutor::~PyExecutor()
{
    // py::finalize_interpreter();
}

void PyExecutor::SetOutputCallback(std::function<void(const std::string&)> callback)
{
    m_Callback = std::move(callback);
    /* Update redirection */
    RedirectStdout();
}

void PyExecutor::RedirectStdout()
{
    py::module sys = py::module::import("sys");
    py::class_<py::object> redirector(py::module::import("__main__"), "VoidPyStdout");

    /**
     * Define the stdout methods
     * write
     * flush
     */
    redirector.def(py::init<>())
        .def("write", [this](py::object, std::string msg)
        {
            if (m_Callback)
                m_Callback(msg);
        })
        .def("flush", [](py::object) {});
    
    m_StdoutRedirector = redirector();
    sys.attr("stdout") = m_StdoutRedirector;
    sys.attr("stderr") = m_StdoutRedirector;
}

void PyExecutor::Execute(const std::string& code)
{
    try
    {
        py::exec(code, m_Globals); 
    }
    catch (py::error_already_set& e)
    {
        if (m_Callback)
            m_Callback(std::string(e.what()));
    }
}

VOID_NAMESPACE_CLOSE
