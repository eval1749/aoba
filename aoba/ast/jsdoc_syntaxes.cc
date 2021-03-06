// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <tuple>

#include "aoba/ast/jsdoc_syntaxes.h"

#include "aoba/ast/node.h"
#include "aoba/ast/node_traversal.h"

namespace aoba {
namespace ast {

//
// JsDocDocument
//
JsDocDocument::JsDocDocument()
    : SyntaxTemplate(std::tuple<>(),
                     SyntaxCode::JsDocDocument,
                     Format::Builder().set_is_variadic(true).Build()) {}

JsDocDocument::~JsDocDocument() = default;

//
// JsDocTag
//
JsDocTag::JsDocTag()
    : SyntaxTemplate(
          std::tuple<>(),
          SyntaxCode::JsDocTag,
          Format::Builder().set_arity(1).set_is_variadic(true).Build()) {}

JsDocTag::~JsDocTag() = default;

const Node& JsDocTag::NameOf(const Node& node) {
  DCHECK_EQ(node, SyntaxCode::JsDocTag);
  return node.child_at(0);
}

ChildNodes JsDocTag::OperandsOf(const Node& node) {
  DCHECK_EQ(node, SyntaxCode::JsDocTag);
  return ast::NodeTraversal::ChildNodesFrom(node, 1);
}

//
// JsDocText
//
JsDocText::JsDocText()
    : SyntaxTemplate(std::tuple<>(),
                     SyntaxCode::JsDocText,
                     Format::Builder().Build()) {}

JsDocText::~JsDocText() = default;

}  // namespace ast
}  // namespace aoba
