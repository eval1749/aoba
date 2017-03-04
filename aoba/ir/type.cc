// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/ir/type.h"

namespace aoba {
namespace ir {

//
// Type
//
Type::Type() = default;
Type::~Type() = default;

bool Type::operator==(const Type& other) const {
  return this == &other;
}

bool Type::operator!=(const Type& other) const {
  return !operator==(other);
}

}  // namespace ir
}  // namespace aoba
