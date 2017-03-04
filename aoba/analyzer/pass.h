// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_PASS_H_
#define AOBA_ANALYZER_PASS_H_

#include "aoba/analyzer/context_user.h"

namespace aoba {

namespace ast {
class Node;
}

class SourceCodeRange;

namespace analyzer {

class Context;
enum class ErrorCode;
class Factory;

//
// Pass
//
class Pass : public ContextUser {
 public:
  virtual ~Pass();

  virtual void RunOnAll();
  virtual void RunOn(const ast::Node& node) = 0;

 protected:
  explicit Pass(Context* context);

 private:
  DISALLOW_COPY_AND_ASSIGN(Pass);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_PASS_H_
