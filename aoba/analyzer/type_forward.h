// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_TYPE_FORWARD_H_
#define AOBA_ANALYZER_TYPE_FORWARD_H_

namespace aoba {
namespace analyzer {

#define FOR_EACH_ABSTRACT_ANALYZE_TYPE(name) \
  V(NamedType)                               \
  V(Type)

#define FOR_EACH_ANALYZE_TYPE(name) \
  V(AnyType)                        \
  V(ClassType)                      \
  V(FunctionType)                   \
  V(InvalidType)                    \
  V(LabeledType)                    \
  V(NilType)                        \
  V(NullType)                       \
  V(PrimitiveType)                  \
  V(RecordType)                     \
  V(TupleType)                      \
  V(TypeAlias)                      \
  V(TypeName)                       \
  V(TypeParameter)                  \
  V(UnionType)                      \
  V(UnspecifiedType)                \
  V(VoidType)

#define V(name) class name;
FOR_EACH_ABSTRACT_ANALYZE_TYPE(V)
FOR_EACH_ANALYZE_TYPE(V)
#undef V

struct FunctionTypeArity;

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_TYPE_FORWARD_H_
