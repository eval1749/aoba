// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_VALUE_FORWARD_H_
#define AOBA_ANALYZER_VALUE_FORWARD_H_

namespace aoba {
namespace analyzer {

#define FOR_EACH_ANALYZE_PRIMITIVE_TYPES(V) \
  V(boolean)                                \
  V(function)                               \
  V(null)                                   \
  V(number)                                 \
  V(object)                                 \
  V(string) /* NOLINT */                    \
  V(symbol)                                 \
  V(undefined)                              \
  V(unknown)                                \
  V(void)

#define FOR_EACH_ABSTRACT_ANALYZE_VALUE(V) \
  V(Class)                                 \
  V(Object)                                \
  V(Value)                                 \
  V(ValueHolder)

#define FOR_EACH_ANALYZE_VALUE(V) \
  V(ConstructedClass)             \
  V(Function)                     \
  V(GenericClass)                 \
  V(NormalClass)                  \
  V(OrdinaryObject)               \
  V(Property)                     \
  V(Undefined)                    \
  V(Variable)

// Forward declarations
enum class ClassKind;

#define V(name) class name;
FOR_EACH_ABSTRACT_ANALYZE_VALUE(V)
FOR_EACH_ANALYZE_VALUE(V)
#undef V

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_VALUE_FORWARD_H_
