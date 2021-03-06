// Copyright (c) 2017 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/analyzer/properties_editor.h"

#include "aoba/analyzer/values.h"
#include "aoba/ast/tokens.h"

namespace aoba {
namespace analyzer {

Properties::Editor::Editor() = default;
Properties::Editor::~Editor() = default;

void Properties::Editor::Add(Properties* properties, const Property& property) {
  if (property.key() == ast::SyntaxCode::Name) {
    const auto& result = properties->name_map_.emplace(
        ast::Name::IdOf(property.key()), &property);
    DCHECK(result.second);
    return;
  }
  const auto& string_key = property.key().range().GetString();
  const auto& result =
      properties->computed_name_map_.emplace(string_key, &property);
  DCHECK(result.second);
}

}  // namespace analyzer
}  // namespace aoba
