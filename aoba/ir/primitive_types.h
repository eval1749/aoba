// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_IR_PRIMITIVE_TYPES_H_
#define AOBA_IR_PRIMITIVE_TYPES_H_

#include "aoba/ir/type.h"

namespace aoba {
namespace ir {

#define V(capital, underscoe)                                        \
  class AOBA_IR_EXPORT capital##Type final : public PrimitiveType { \
    DECLARE_CONCRETE_IR_TYPE(capital##Type, PrimitiveType);          \
                                                                     \
   public:                                                           \
    ~capital##Type();                                                \
                                                                     \
   private:                                                          \
    friend class PrimitiveTypeFactory;                               \
                                                                     \
    capital##Type();                                                 \
                                                                     \
    DISALLOW_COPY_AND_ASSIGN(capital##Type);                         \
  };

//
// PrimitiveType
//
class AOBA_IR_EXPORT PrimitiveType : public Type {
  DECLARE_ABSTRACT_IR_TYPE(PrimitiveType, Type);

 public:
  ~PrimitiveType();

 protected:
  PrimitiveType();

 private:
  DISALLOW_COPY_AND_ASSIGN(PrimitiveType);
};

FOR_EACH_IR_PRIMITIVE_TYPE(V)
#undef V

}  // namespace ir
}  // namespace aoba

#endif  // AOBA_IR_PRIMITIVE_TYPES_H_
