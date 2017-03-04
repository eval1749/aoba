// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_IR_TYPE_H_
#define AOBA_IR_TYPE_H_

#include <iosfwd>

#include "base/macros.h"
#include "aoba/base/castable.h"
#include "aoba/base/memory/zone_allocated.h"
#include "aoba/ir/ir_export.h"
#include "aoba/ir/type_forward.h"

namespace aoba {
namespace ir {

class PrimitiveTypeFactory;
class TypeFactory;

#define DECLARE_IR_TYPE(name, base) DECLARE_CASTABLE_CLASS(name, base);

#define DECLARE_ABSTRACT_IR_TYPE(name, base) DECLARE_IR_TYPE(name, base);

#define DECLARE_CONCRETE_IR_TYPE(name, base) \
  DECLARE_IR_TYPE(name, base);               \
  friend class TypeFactory;

//
// Type
//
class AOBA_IR_EXPORT Type : public Castable<Type>, public ZoneAllocated {
  DECLARE_ABSTRACT_IR_TYPE(Type, Castable);

 public:
  ~Type();

  bool operator==(const Type& other) const;
  bool operator!=(const Type& other) const;

 protected:
  Type();

 private:
  DISALLOW_COPY_AND_ASSIGN(Type);
};

}  // namespace ir
}  // namespace aoba

#endif  // AOBA_IR_TYPE_H_
