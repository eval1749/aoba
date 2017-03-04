// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_TESTING_PRINT_AS_TREE_H_
#define AOBA_TESTING_PRINT_AS_TREE_H_

#include <iosfwd>

namespace aoba {

namespace ast {
class Node;
}

namespace internal {
struct PrintableAstNode {
  const ast::Node* node;
};

std::ostream& operator<<(std::ostream& ostream,
                         const PrintableAstNode& printable);

}  // namespace internal

internal::PrintableAstNode AsPrintableTree(const ast::Node& node);

}  // namespace aoba

#endif  // AOBA_TESTING_PRINT_AS_TREE_H_
