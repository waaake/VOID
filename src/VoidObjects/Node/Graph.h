// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _CORE_GRAPH_H
#define _CORE_GRAPH_H

/* STD */
#include <vector>

/* Internal */
#include "Definition.h"
#include "Node.h"

VOID_NAMESPACE_OPEN

namespace Core {

class Graph
{
    Graph() = default;

public:
    static Graph& Instance();

    ~Graph();

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    Graph(Graph&&) = delete;
    Graph& operator=(Graph&&) = delete;

    void AddNode(const SharedNode& node);
    BufferData Evaluate(const SharedNode& node, v_frame_t frame);

private: /* Members */
    std::vector<SharedNode> m_Nodes;
};

} // namespace Core

VOID_NAMESPACE_CLOSE

#endif // _CORE_GRAPH_H
