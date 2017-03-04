// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/analyzer/value_editor.h"

#include "aoba/analyzer/built_in_world.h"
#include "aoba/analyzer/values.h"
#include "aoba/ast/bindings.h"
#include "aoba/ast/declarations.h"
#include "aoba/ast/expressions.h"
#include "aoba/ast/jsdoc_syntaxes.h"
#include "aoba/ast/statements.h"

namespace aoba {
namespace analyzer {

//
// Value::Editor
//
Value::Editor::Editor() = default;
Value::Editor::~Editor() = default;

void Value::Editor::AddAssignment(const ValueHolder& binding,
                                  const Value& value) {
  const_cast<ValueHolder&>(binding).data_.assignments_.push_back(&value);
}

void Value::Editor::SetClassHeritage(const Class& class_value,
                                     const std::vector<const Class*>& classes) {
  auto& base_classes = const_cast<Class&>(class_value).base_classes_;
  DCHECK(base_classes.empty()) << class_value;
  base_classes.reserve(classes.size());
  base_classes.insert(base_classes.begin(), classes.begin(), classes.end());
}

void Value::Editor::SetClassList(const Class& class_value,
                                 const std::vector<const Class*>& classes) {
  auto& class_list = const_cast<Class&>(class_value).class_list_;
  DCHECK(class_list.empty()) << class_value;
  class_list.reserve(classes.size());
  class_list.insert(class_list.begin(), classes.begin(), classes.end());
}

}  // namespace analyzer
}  // namespace aoba
