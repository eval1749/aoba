// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_AST_NODE_PRINTER_H_
#define AOBA_AST_NODE_PRINTER_H_

#include <iosfwd>

#include "aoba/ast/ast_export.h"

namespace aoba {
namespace ast {

class Node;

struct NodeAsSourceCode {
  char delimiter;
  const Node* node;
};

AOBA_AST_EXPORT NodeAsSourceCode AsSourceCode(const Node& node,
                                               char delimiter = 0);

AOBA_AST_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                          const NodeAsSourceCode& printable);

}  // namespace ast
}  // namespace aoba

#endif  // AOBA_AST_NODE_PRINTER_H_
