// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <string>

#include "Definition.h"

VOID_NAMESPACE_OPEN

class Op
{
public:
    Op(const std::string& name) : m_Name(name) {}

protected:
    std::string m_Name;
};

// class MediaReaderOp : public Op
// {
// public:
//     MediaReaderOp(const std::string)
// };

VOID_NAMESPACE_CLOSE
