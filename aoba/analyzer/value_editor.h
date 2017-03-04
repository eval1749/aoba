// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_VALUE_EDITOR_H_
#define AOBA_ANALYZER_VALUE_EDITOR_H_

#include <vector>

#include "aoba/analyzer/value.h"

namespace aoba {
namespace analyzer {

class Class;
class Function;
class ValueHolder;

//
// Value::Editor
//
class Value::Editor final {
 public:
  Editor();
  ~Editor();

  void AddAssignment(const ValueHolder& binding, const Value& value);
  void SetClassHeritage(const Class& class_value,
                        const std::vector<const Class*>& classes);
  void SetClassList(const Class& class_value,
                    const std::vector<const Class*>& classes);

 private:
  DISALLOW_COPY_AND_ASSIGN(Editor);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_VALUE_EDITOR_H_
