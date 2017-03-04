// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/analyzer/value.h"

namespace aoba {
namespace analyzer {

//
// Value
//
Value::Value(int id, const ast::Node& node) : id_(id), node_(node) {}
Value::~Value() = default;

bool Value::operator==(const Value& other) const {
  return this == &other;
}

bool Value::operator==(const Value* other) const {
  return this == other;
}

bool Value::operator!=(const Value& other) const {
  return this != &other;
}

bool Value::operator!=(const Value* other) const {
  return this != other;
}

}  // namespace analyzer
}  // namespace aoba
