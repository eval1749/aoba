// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stack>
#include <utility>

#include "aoba/analyzer/type_checker.h"

#include "aoba/analyzer/context.h"
#include "aoba/analyzer/error_codes.h"
#include "aoba/analyzer/factory.h"
#include "aoba/analyzer/types.h"
#include "aoba/analyzer/values.h"
#include "aoba/ast/expressions.h"
#include "aoba/ast/node.h"
#include "aoba/ast/node_traversal.h"
#include "aoba/ast/types.h"

namespace aoba {
namespace analyzer {

//
// TypeChecker
//
TypeChecker::TypeChecker(Context* context) : Pass(context) {}
TypeChecker::~TypeChecker() = default;

// The entry point
void TypeChecker::RunOn(const ast::Node& toplevel_node) {
  for (const auto& node : ast::NodeTraversal::DescendantsOf(toplevel_node))
    Visit(node);
}

// |ast::SyntaxVisitor| members
// Expressions
void TypeChecker::VisitInternal(const ast::ReferenceExpression& syntax,
                                const ast::Node& node) {
  const auto* value = context().TryValueOf(node);
  if (!value)
    return;
  const auto& variable = value->As<Variable>();
  if (variable.kind() == VariableKind::Parameter ||
      !variable.assignments().empty()) {
    return;
  }
  AddError(node, ErrorCode::TYPE_CHECKER_UNINITIALIZED_VARIABLE);
}

// Types
void TypeChecker::VisitInternal(const ast::TypeName& syntax,
                                const ast::Node& node) {
  const auto* present = context().TryTypeOf(node);
  if (!present)
    return;
}

}  // namespace analyzer
}  // namespace aoba
