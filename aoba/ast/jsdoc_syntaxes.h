// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_AST_JSDOC_SYNTAXES_H_
#define AOBA_AST_JSDOC_SYNTAXES_H_

#include <vector>

#include "aoba/ast/syntax.h"
#include "aoba/ast/syntax_forward.h"

namespace aoba {
namespace ast {

class ChildNodes;
class Node;

//
// JsDocDocument
//
class AOBA_AST_EXPORT JsDocDocument final : public SyntaxTemplate<Syntax> {
  DECLARE_CONCRETE_AST_SYNTAX(JsDocDocument, Syntax);

 public:
  ~JsDocDocument() final;

 private:
  JsDocDocument();

  DISALLOW_COPY_AND_ASSIGN(JsDocDocument);
};

//
// JsDocTag
//
class AOBA_AST_EXPORT JsDocTag final : public SyntaxTemplate<Syntax> {
  DECLARE_CONCRETE_AST_SYNTAX(JsDocTag, Syntax);

 public:
  ~JsDocTag() final;

  static const Node& NameOf(const Node& node);
  static ChildNodes OperandsOf(const Node& node);

 private:
  JsDocTag();

  DISALLOW_COPY_AND_ASSIGN(JsDocTag);
};

//
// JsDocText
//
class AOBA_AST_EXPORT JsDocText final : public SyntaxTemplate<Syntax> {
  DECLARE_CONCRETE_AST_SYNTAX(JsDocText, Syntax);

 public:
  ~JsDocText() final;

 private:
  JsDocText();

  DISALLOW_COPY_AND_ASSIGN(JsDocText);
};

}  // namespace ast
}  // namespace aoba

#endif  // AOBA_AST_JSDOC_SYNTAXES_H_
