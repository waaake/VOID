// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _CORE_NODE_H
#define _CORE_NODE_H

/* STD */
#include <string>
#include <memory>

/* Internal */
#include "Definition.h"
#include "Data.h"

VOID_NAMESPACE_OPEN

namespace Core {

class Node;
typedef std::shared_ptr<Node> SharedNode;

class Node
{
public:
    Node(const std::string& name);
    virtual ~Node();

    virtual std::string Class() const noexcept { return "Node"; }
    virtual BufferData Evaluate(v_frame_t frame);

private:
    std::string m_Name;
};

} // namespace Core

VOID_NAMESPACE_CLOSE

#endif // _CORE_NODE_H
