// Copyright 2014-2015 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <array>
#include <sstream>

#include "aoba/base/graphs/graph_editor.h"
#include "aoba/base/graphs/graph_test_support.h"

namespace aoba {
namespace testing {

// Function
std::string Function::ToString() const {
  std::ostringstream ostream;
  ostream << *this;
  return ostream.str();
}

// Printable
PrintableBlocks::PrintableBlocks(const PrintableBlocks&) = default;
PrintableBlocks::PrintableBlocks() = default;
PrintableBlocks::~PrintableBlocks() = default;

PrintableBlocks Printable(const ZoneUnorderedSet<Block*>& blocks) {
  PrintableBlocks printable;
  printable.blocks.insert(printable.blocks.end(), blocks.begin(), blocks.end());
  std::sort(printable.blocks.begin(), printable.blocks.end(),
            [](Block* a, Block* b) { return a->id() < b->id(); });
  return printable;
}

PrintableBlocks Printable(const ZoneVector<Block*>& blocks) {
  PrintableBlocks printable;
  printable.blocks.insert(printable.blocks.end(), blocks.begin(), blocks.end());
  std::sort(printable.blocks.begin(), printable.blocks.end(),
            [](Block* a, Block* b) { return a->id() < b->id(); });
  return printable;
}

std::ostream& operator<<(std::ostream& ostream, const PrintableBlocks& blocks) {
  ostream << "{";
  auto* separator = "";
  for (auto* block : blocks.blocks) {
    ostream << separator << block;
    separator = ", ";
  }
  return ostream << "}";
}

std::ostream& operator<<(std::ostream& ostream, const Block& block) {
  ostream << "{id: " << block.id()
          << ", predecessors: " << Printable(block.predecessors())
          << ", successors: " << Printable(block.successors()) << "}";
  return ostream;
}

std::ostream& operator<<(std::ostream& ostream, const Block* block) {
  if (!block)
    return ostream << "nil";
  if (block->id() == -1)
    return ostream << "ENTRY";
  if (block->id() == -2)
    return ostream << "EXIT";
  return ostream << "B" << block->id();
}

std::ostream& operator<<(std::ostream& ostream, const Function& function) {
  for (auto* block : function.nodes())
    ostream << *block << std::endl;
  return ostream;
}

GraphTestBase::GraphTestBase() : ZoneOwner("GraphTestBase") {}
GraphTestBase::~GraphTestBase() = default;

Block* GraphTestBase::NewBlock(int id) {
  auto* block = new (zone()) Block(zone(), id);
  blocks_.push_back(block);
  return block;
}

// Build graph
//      1
//     / \
//     2  3
//     \  /
//      4
void GraphTestBase::MakeDiamondGraph() {
  auto* block1 = NewBlock(1);
  auto* block2 = NewBlock(2);
  auto* block3 = NewBlock(3);
  auto* block4 = NewBlock(4);

  Function::Editor().AppendNode(&function(), block1)
      .AppendNode(&function(), block2)
      .AppendNode(&function(), block3)
      .AppendNode(&function(), block4)
      .AddEdge(&function(), block1, block2)
      .AddEdge(&function(), block1, block3)
      .AddEdge(&function(), block2, block4)
      .AddEdge(&function(), block3, block4);
}

//      B0---------+    B0 -> B1, B5
//      |          |
//      B1<------+ |    B1 -> B2, B4
//      |        | |
//   +->B2-->B5  | |    B2 -> B3, B6
//   |  |    |   | |
//   +--B3<--+   | |    B3 -> B2, B4
//      |        | |
//      B4<------+ |    B4 -> B1, B6
//      |          |    B5 -> B3
//      B6<--------+    B6
//
//  B0: parent=ENTRY children=[B1, B5]
//  B1: parent=B0    children=[B2, B4]
//  B2: parent=B1    children=[B2, B3]
//  B3: parent=B2    children=[]
//  B4: parent=B1    children=[]
//  B5: parent=B2    children=[]
//  B6: parent=B0    children=[EXIT]
void GraphTestBase::MakeSampleGraph1() {
  Function::Editor editor;

  auto* entry_block = NewBlock(-1);
  auto* exit_block = NewBlock(-2);

  editor.AppendNode(&function(), entry_block);
  std::array<Block*, 7> blocks;
  auto id = 0;
  for (auto* ref : blocks) {
    ref = NewBlock(id);
    editor.AppendNode(&function(), blocks[id]);
    ++id;
  }
  editor.AppendNode(&function(), exit_block);

  editor.AddEdge(&function(), entry_block, blocks[0]);

  editor.AddEdge(&function(), blocks[0], blocks[1]);
  editor.AddEdge(&function(), blocks[0], blocks[6]);

  editor.AddEdge(&function(), blocks[1], blocks[2]);
  editor.AddEdge(&function(), blocks[1], blocks[4]);

  editor.AddEdge(&function(), blocks[2], blocks[3]);
  editor.AddEdge(&function(), blocks[2], blocks[5]);

  editor.AddEdge(&function(), blocks[3], blocks[2]);
  editor.AddEdge(&function(), blocks[3], blocks[4]);

  editor.AddEdge(&function(), blocks[4], blocks[1]);
  editor.AddEdge(&function(), blocks[4], blocks[6]);

  editor.AddEdge(&function(), blocks[5], blocks[3]);

  editor.AddEdge(&function(), blocks[6], exit_block);
}

std::string ToString(const OrderedList<Block*>& list) {
  std::ostringstream ostream;
  ostream << "[";
  auto* separator = "";
  for (auto* block : list) {
    ostream << separator << block;
    separator = ", ";
  }
  ostream << "]";
  return ostream.str();
}

}  // namespace testing
}  // namespace aoba
