// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Graph.h"

VOID_NAMESPACE_OPEN

namespace Core {

Graph& Graph::Instance()
{
    static Graph instance;
    return instance;
}

Graph::~Graph()
{

}

void Graph::AddNode(const SharedNode& node)
{
    m_Nodes.push_back(node);
}

BufferData Graph::Evaluate(const SharedNode& node, v_frame_t frame)
{
    return node->Evaluate(frame);
}

} // namespace Core

VOID_NAMESPACE_CLOSE
