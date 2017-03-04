// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_IR_NODE_FORWARD_H_
#define AOBA_IR_NODE_FORWARD_H_

#include <iosfwd>

#include "aoba/ir/ir_export.h"

namespace aoba {
namespace ir {

class Node;
class Operator;
class Type;

// Implemented in "aoba/ir/node_printer.cc"
AOBA_IR_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                         const Node& node);

AOBA_IR_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                         const Node* node);

}  // namespace ir
}  // namespace aoba

#endif  // AOBA_IR_NODE_FORWARD_H_
