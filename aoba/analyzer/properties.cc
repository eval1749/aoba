// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/analyzer/properties.h"

#include "aoba/analyzer/values.h"
#include "aoba/ast/node.h"
#include "aoba/ast/syntax.h"
#include "aoba/ast/tokens.h"

namespace aoba {
namespace analyzer {

//
// Properties
//
Properties::Properties(Zone* zone, const ast::Node& owner)
    : computed_name_map_(zone), name_map_(zone), owner_(owner) {}

Properties::~Properties() = default;

const Property& Properties::Get(const ast::Node& key) const {
  auto* property = TryGet(key);
  DCHECK(property) << key;
  return *property;
}

const Property* Properties::TryGet(const ast::Node& key) const {
  if (key == ast::SyntaxCode::Name) {
    const auto& it = name_map_.find(ast::Name::IdOf(key));
    return it == name_map_.end() ? nullptr : it->second;
  }
  const auto& string_key = key.range().GetString();
  const auto& it = computed_name_map_.find(string_key);
  return it == computed_name_map_.end() ? nullptr : it->second;
}

}  // namespace analyzer
}  // namespace aoba
