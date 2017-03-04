// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_TYPE_CHECKER_H_
#define AOBA_ANALYZER_TYPE_CHECKER_H_

#include "aoba/analyzer/pass.h"

#include "aoba/ast/syntax_visitor.h"

namespace aoba {

namespace ast {
class ReferenceExpressionSyntax;
}

namespace analyzer {

class Type;
class Variable;

//
// TypeChecker
//
class TypeChecker final : public Pass, public ast::SyntaxVisitor {
 public:
  explicit TypeChecker(Context* context);
  ~TypeChecker() final;

  void RunOn(const ast::Node& node) final;

 private:
  // |ast::SyntaxVisitor| members
  // Expressions
  void VisitInternal(const ast::ReferenceExpression& syntax,
                     const ast::Node& node) final;

  // Types
  void VisitInternal(const ast::TypeName& syntax, const ast::Node& node) final;

  DISALLOW_COPY_AND_ASSIGN(TypeChecker);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_TYPE_CHECKER_H_
