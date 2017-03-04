// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_PRINT_AS_TREE_H_
#define AOBA_ANALYZER_PRINT_AS_TREE_H_

#include <iosfwd>

namespace aoba {

namespace ast {
class Node;
}

namespace analyzer {

class Context;

struct PrintableTreeNode {
  const Context* context;
  const ast::Node* node;
};

PrintableTreeNode AsPrintableTree(const Context& context,
                                  const ast::Node& node);

std::ostream& operator<<(std::ostream& ostream,
                         const PrintableTreeNode& printable);

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_PRINT_AS_TREE_H_
