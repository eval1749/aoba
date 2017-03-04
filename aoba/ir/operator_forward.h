// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_IR_OPERATOR_FORWARD_H_
#define AOBA_IR_OPERATOR_FORWARD_H_

#include <iosfwd>

#include "aoba/base/float_type.h"
#include "aoba/ir/ir_export.h"
#include "aoba/ir/type_forward.h"

namespace aoba {
namespace ir {

#define FOR_EACH_COMMON_IR_OPERATOR(V) \
  V(Exit)                              \
  V(If)                                \
  V(IfException)                       \
  V(IfFalse)                           \
  V(IfSuccess)                         \
  V(IfTrue)                            \
  V(LiteralBool)                       \
  V(LiteralFloat64)                    \
  V(LiteralInt64)                      \
  V(LiteralString)                     \
  V(LiteralVoid)                       \
  V(Projection)                        \
  V(Ret)                               \
  V(Start)                             \
  V(Tuple)

#define FOR_EACH_IR_OPERATOR(V) FOR_EACH_COMMON_IR_OPERATOR(V)

class Operator;
enum class OperationCode;
#define V(name) class name##Operator;
FOR_EACH_IR_OPERATOR(V)
#undef V

// Implemented in "aoba/ir/operator_printer.cc"
AOBA_IR_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                         const Operator& op);

AOBA_IR_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                         const Operator* op);

}  // namespace ir
}  // namespace aoba

#endif  // AOBA_IR_OPERATOR_FORWARD_H_
