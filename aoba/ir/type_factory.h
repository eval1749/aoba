// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_IR_TYPE_FACTORY_H_
#define AOBA_IR_TYPE_FACTORY_H_

#include <memory>
#include <vector>

#include "base/macros.h"
#include "aoba/ir/ir_export.h"
#include "aoba/ir/type_forward.h"

namespace aoba {

class Zone;

namespace ir {

class CompositeTypeFactory;

//
// TypeFactory
//
class AOBA_IR_EXPORT TypeFactory final {
 public:
  explicit TypeFactory(Zone* zone);
  ~TypeFactory();

#define V(capital, underscore) const Type& underscore##_type() const;
  FOR_EACH_IR_PRIMITIVE_TYPE(V)
#undef V

  const Type& NewFunctionType(const Type& parameters_type,
                              const Type& return_type);

  const Type& NewReferenceType(const Type& to);

  const Type& NewTupleTypeFromVector(const std::vector<const Type*>& members);

  template <typename... Members>
  const Type& NewTupleType(const Members&... members) {
    return NewTupleTypeFromVector({&members...});
  }

  const Type& NewUnionType(const std::vector<const Type*>& members);

 private:
  const std::unique_ptr<CompositeTypeFactory> composite_type_factory_;

  DISALLOW_COPY_AND_ASSIGN(TypeFactory);
};

}  // namespace ir
}  // namespace aoba

#endif  // AOBA_IR_TYPE_FACTORY_H_
