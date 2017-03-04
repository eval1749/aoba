// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_AST_LITERALS_H_
#define AOBA_AST_LITERALS_H_

#include "aoba/ast/expressions.h"
#include "aoba/base/float_type.h"

namespace aoba {
namespace ast {

//
// Literal
//
class AOBA_AST_EXPORT Literal : public Expression {
  DECLARE_ABSTRACT_AST_SYNTAX(Literal, Expression);

 public:
  ~Literal() override;

 protected:
  Literal(SyntaxCode syntax_code, const Format& format);

 private:
  DISALLOW_COPY_AND_ASSIGN(Literal);
};

//
// BooleanLiteral
//
class AOBA_AST_EXPORT BooleanLiteral final
    : public SyntaxTemplate<Expression, bool> {
  DECLARE_CONCRETE_AST_SYNTAX(BooleanLiteral, Expression);

 public:
  ~BooleanLiteral() final;

 private:
  explicit BooleanLiteral(bool value);

  DISALLOW_COPY_AND_ASSIGN(BooleanLiteral);
};

//
// NullLiteral
//
class AOBA_AST_EXPORT NullLiteral final : public SyntaxTemplate<Expression> {
  DECLARE_CONCRETE_AST_SYNTAX(NullLiteral, Expression);

 public:
  ~NullLiteral() final;

 private:
  NullLiteral();

  DISALLOW_COPY_AND_ASSIGN(NullLiteral);
};

//
// NumericLiteral
//
class AOBA_AST_EXPORT NumericLiteral final
    : public SyntaxTemplate<Expression, float64_t> {
  DECLARE_CONCRETE_AST_SYNTAX(NumericLiteral, Expression);

 public:
  ~NumericLiteral() final;

 private:
  explicit NumericLiteral(float64_t value);

  DISALLOW_COPY_AND_ASSIGN(NumericLiteral);
};

//
// StringLiteral
//
class AOBA_AST_EXPORT StringLiteral final : public SyntaxTemplate<Expression> {
  DECLARE_CONCRETE_AST_SYNTAX(StringLiteral, Expression);

 public:
  ~StringLiteral() final;

 private:
  StringLiteral();

  DISALLOW_COPY_AND_ASSIGN(StringLiteral);
};

//
// UndefinedLiteral
//
class AOBA_AST_EXPORT UndefinedLiteral final
    : public SyntaxTemplate<Expression> {
  DECLARE_CONCRETE_AST_SYNTAX(UndefinedLiteral, Expression);

 public:
  ~UndefinedLiteral() final;

 private:
  UndefinedLiteral();

  DISALLOW_COPY_AND_ASSIGN(UndefinedLiteral);
};

}  // namespace ast
}  // namespace aoba

#endif  // AOBA_AST_LITERALS_H_
