// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_CORE_PYTHON_EXECUTOR_H
#define _VOID_CORE_PYTHON_EXECUTOR_H

/* Conflicts with PyType_Slot* slots */
#ifdef slots
#undef slots
#endif

/* STD */
#include <functional>
#include <string>

/* Pybind11 */
#include <pybind11/embed.h>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VOID_API PyExecutor
{
public:
    PyExecutor();
    ~PyExecutor();

    void SetOutputCallback(std::function<void(const std::string&)> callback);
    void Execute(const std::string& code);

private: /* Members */
    pybind11::scoped_interpreter m_Guard;
    pybind11::object m_Globals;
    pybind11::object m_StdoutRedirector;

    std::function<void(const std::string&)> m_Callback;

private: /* Methods */
    void RedirectStdout();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_CORE_PYTHON_EXECUTOR_H
