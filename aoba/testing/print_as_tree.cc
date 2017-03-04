// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ostream>
#include <type_traits>

#include "base/auto_reset.h"
#include "base/macros.h"
#include "base/strings/string_piece.h"
#include "base/strings/utf_string_conversions.h"
#include "aoba/ast/bindings.h"
#include "aoba/ast/expressions.h"
#include "aoba/ast/jsdoc_syntaxes.h"
#include "aoba/ast/node.h"
#include "aoba/ast/node_traversal.h"
#include "aoba/ast/syntax.h"
#include "aoba/ast/tokens.h"
#include "aoba/ast/types.h"
#include "aoba/testing/print_as_tree.h"

namespace aoba {
namespace internal {

namespace {

//
// Context
//
struct Context {
  int level = 0;
};

//
// Indent
//
struct Indent {
  explicit Indent(const Context& context) : level(context.level) {}
  explicit Indent(int passed_level) : level(passed_level) {}
  int level;
};

std::ostream& operator<<(std::ostream& ostream, const Indent& indent) {
  ostream << std::endl;
  if (indent.level == 0)
    return ostream;
  for (auto counter = 0; counter < indent.level - 1; ++counter)
    ostream << "|  ";
  ostream << "+--";
  return ostream;
}

//
// IndentScope
//
class IndentScope final {
 public:
  explicit IndentScope(Context* context)
      : level_holder_(&context->level, context->level + 1) {}

  ~IndentScope() = default;

 private:
  base::AutoReset<int> level_holder_;

  DISALLOW_COPY_AND_ASSIGN(IndentScope);
};

//
// UsingSourceCode is a wrapper of AST node to print using source code.
//
struct UsingSourceCode {
  const ast::Node* node;
};

UsingSourceCode SourceCodeOf(const ast::Node& node) {
  return UsingSourceCode{&node};
}

std::ostream& operator<<(std::ostream& ostream,
                         const UsingSourceCode& printable) {
  const auto& node = *printable.node;
  return ostream << base::UTF16ToUTF8(node.range().GetString());
}

// Dispatcher
template <typename T>
struct Printable {
  Context* context;
  const T* node;
};

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<ast::Node>& printable) {
  auto& context = *printable.context;
  const auto& node = *printable.node;
  ostream << node.syntax();
  if (node.arity() == 0)
    return ostream << " |" << SourceCodeOf(node) << '|';
  IndentScope scope(&context);
  for (const auto& child : ast::NodeTraversal::ChildNodesOf(node))
    ostream << Indent(context) << Printable<ast::Node>{&context, &child};
  return ostream;
}

}  // namespace

std::ostream& operator<<(std::ostream& ostream,
                         const PrintableAstNode& printable) {
  Context context;
  return ostream << Printable<ast::Node>{&context, printable.node};
}

}  // namespace internal

internal::PrintableAstNode AsPrintableTree(const ast::Node& node) {
  return internal::PrintableAstNode{&node};
}

}  // namespace aoba
