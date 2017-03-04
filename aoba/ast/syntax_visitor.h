// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_AST_SYNTAX_VISITOR_H_
#define AOBA_AST_SYNTAX_VISITOR_H_

#include "base/macros.h"
#include "aoba/ast/ast_export.h"
#include "aoba/ast/syntax_forward.h"

namespace aoba {
namespace ast {

class Node;

//
// SyntaxVisitor
//
class AOBA_AST_EXPORT SyntaxVisitor {
 public:
  ~SyntaxVisitor();

  void Visit(const ast::Node& node);
  virtual void VisitDefault(const ast::Node& node);

#define V(name) \
  virtual void VisitInternal(const name& syntax, const ast::Node& node);
  FOR_EACH_AST_SYNTAX(V)
#undef V

 protected:
  SyntaxVisitor();

 private:
  DISALLOW_COPY_AND_ASSIGN(SyntaxVisitor);
};

void AOBA_AST_EXPORT DepthFirstTraverse(SyntaxVisitor* visitor,
                                         const Node& start_node);

}  // namespace ast
}  // namespace aoba

#endif  // AOBA_AST_SYNTAX_VISITOR_H_
