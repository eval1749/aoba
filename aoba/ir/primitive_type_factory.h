// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_IR_PRIMITIVE_TYPE_FACTORY_H_
#define AOBA_IR_PRIMITIVE_TYPE_FACTORY_H_

#include "base/macros.h"
#include "aoba/base/memory/zone.h"
#include "aoba/ir/ir_export.h"
#include "aoba/ir/type_forward.h"

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}

namespace aoba {

class Zone;

namespace ir {

//
// PrimitiveTypeFactory
//
class PrimitiveTypeFactory final {
 public:
  static PrimitiveTypeFactory* GetInstance();

#define V(capital, underscore) \
  const Type& underscore##_type() const { return underscore##_type_; }
  FOR_EACH_IR_PRIMITIVE_TYPE(V)
#undef V

 private:
  friend struct base::DefaultSingletonTraits<PrimitiveTypeFactory>;

  PrimitiveTypeFactory();
  ~PrimitiveTypeFactory() = default;

  Zone zone_;

#define V(capital, underscore) const Type& underscore##_type_;
  FOR_EACH_IR_PRIMITIVE_TYPE(V)
#undef V
  int dummy_;

  DISALLOW_COPY_AND_ASSIGN(PrimitiveTypeFactory);
};

}  // namespace ir
}  // namespace aoba

#endif  // AOBA_IR_PRIMITIVE_TYPE_FACTORY_H_
