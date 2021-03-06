// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/ir/primitive_type_factory.h"

#include "base/memory/singleton.h"
#include "aoba/ir/primitive_types.h"
#include "aoba/ir/type.h"

namespace aoba {
namespace ir {

#define V(capital, underscore) \
  underscore##_type_(*new (&zone_) capital##Type()),
PrimitiveTypeFactory::PrimitiveTypeFactory()
    : zone_("PrimitiveTypeFactory"), FOR_EACH_IR_PRIMITIVE_TYPE(V) dummy_(0) {}
#undef V

PrimitiveTypeFactory* PrimitiveTypeFactory::GetInstance() {
  return base::Singleton<PrimitiveTypeFactory>::get();
}

}  // namespace ir
}  // namespace aoba
