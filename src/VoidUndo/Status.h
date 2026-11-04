// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _COMMAND_STATUS_H
#define _COMMAND_STATUS_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/// @brief Provides the status of the Undo Command at any given time
/// If the undo command is created and hasn't been performed the status is Unknown
/// Once the command is executed, the status can be either a Failure (false) or Success (true)
/// Provides an overloaded bool operator for ease in checking whether the command is successful or not
struct CommandStatus
{
    enum class Code
    {
        Unknown = -1,
        Failure,
        Success
    };

    Code code { Code::Unknown };
    constexpr CommandStatus() {}
    constexpr CommandStatus(bool status) : code(status ? Code::Success : Code::Failure) {}

    constexpr inline operator bool() const { return code == Code::Success; }
    constexpr inline bool Failed() const { return code == Code::Failure; }
};

VOID_NAMESPACE_CLOSE

#endif // _COMMAND_STATUS_H
