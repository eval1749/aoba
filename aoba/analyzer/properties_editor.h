// Copyright (c) 2017 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_PROPERTIES_EDITOR_H_
#define AOBA_ANALYZER_PROPERTIES_EDITOR_H_

#include "aoba/analyzer/properties.h"

namespace aoba {
namespace analyzer {

//
// Properties::Editor
//
class Properties::Editor final {
 public:
  Editor();
  ~Editor();

  void Add(Properties* properties, const Property& property);

 private:
  DISALLOW_COPY_AND_ASSIGN(Editor);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_PROPERTIES_EDITOR_H_
