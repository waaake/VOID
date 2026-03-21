// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Node.h"

VOID_NAMESPACE_OPEN

namespace Core {

Node::Node(const std::string& name)
    : m_Name(name)
{
}

Node::~Node()
{
}

BufferData Node::Evaluate(v_frame_t frame)
{
    return BufferData();
}

} // namespace Core

VOID_NAMESPACE_CLOSE
