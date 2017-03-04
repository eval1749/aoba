// Copyright 2014-2015 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_GRAPHS_FLOW_GRAPH_H_
#define AOBA_BASE_GRAPHS_FLOW_GRAPH_H_

namespace aoba {

// BackwardFlowGraph is used for computing post-dominator tree
template <typename Graph>
struct BackwardFlowGraph {
  using GraphNode = typename Graph::GraphNode;

  static GraphNode* EntryOf(const Graph& graph) { return graph.last_node(); }

  static bool HasMoreThanOnePredecessor(const GraphNode* node) {
    return node->HasMoreThanOneSuccessor();
  }

  static bool HasMoreThanOneSuccessor(const GraphNode* node) {
    return node->HasMoreThanOnePredecessor();
  }

  static typename Graph::NodeList PredecessorsOf(const GraphNode* node) {
    return node->successors();
  }

  static typename Graph::NodeList SuccessorsOf(const GraphNode* node) {
    return node->predecessors();
  }
};

// ForwardFlowGraph is used for computing dominator tree
template <typename Graph>
struct ForwardFlowGraph {
  using GraphNode = typename Graph::GraphNode;

  static GraphNode* EntryOf(const Graph& graph) { return graph.first_node(); }

  static bool HasMoreThanOnePredecessor(const GraphNode* node) {
    return node->HasMoreThanOnePredecessor();
  }

  static bool HasMoreThanOneSuccessor(const GraphNode* node) {
    return node->HasMoreThanOneSuccessor();
  }

  static typename Graph::NodeList PredecessorsOf(const GraphNode* node) {
    return node->predecessors();
  }

  static typename Graph::NodeList SuccessorsOf(const GraphNode* node) {
    return node->successors();
  }
};

}  // namespace aoba

#endif  // AOBA_BASE_GRAPHS_FLOW_GRAPH_H_
