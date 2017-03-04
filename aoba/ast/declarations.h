// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_AST_DECLARATIONS_H_
#define AOBA_AST_DECLARATIONS_H_

#include <iosfwd>

#include "aoba/ast/node.h"
#include "aoba/ast/syntax.h"
#include "aoba/ast/syntax_forward.h"

namespace aoba {
namespace ast {

//
// FunctionKind
//
#define FOR_EACH_AST_FUNCTION_KIND(V) \
  V(Invalid)                          \
  V(Async)                            \
  V(Generator)                        \
  V(Getter)                           \
  V(Normal)                           \
  V(Setter)

enum class FunctionKind {
#define V(name) name,
  FOR_EACH_AST_FUNCTION_KIND(V)
#undef V
};

AOBA_AST_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                          FunctionKind kind);

//
// MethodKind
//
#define FOR_EACH_AST_METHOD_KIND(V) \
  V(NonStatic)                      \
  V(Static)

enum class MethodKind {
#define V(name) name,
  FOR_EACH_AST_METHOD_KIND(V)
#undef V
};

AOBA_AST_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                          MethodKind kind);

//
// Annotation
//
class AOBA_AST_EXPORT Annotation final : public SyntaxTemplate<Syntax> {
  DECLARE_CONCRETE_AST_SYNTAX(Annotation, Syntax);

 public:
  ~Annotation() final;

  static const Node& AnnotatedOf(const Node& node);
  static const Node& DocumentOf(const Node& node);

 private:
  Annotation();

  DISALLOW_COPY_AND_ASSIGN(Annotation);
};

//
// ArrowFunction
//
class AOBA_AST_EXPORT ArrowFunction final
    : public SyntaxTemplate<Syntax, FunctionKind> {
  DECLARE_CONCRETE_AST_SYNTAX(ArrowFunction, Syntax);

 public:
  ~ArrowFunction() final;

  static const Node& BodyOf(const Node& node);

  //  - x = BindingNameElement
  //  - () = ParameterList
  //  - (a, b, ...) = ParameterList
  static const Node& ParametersOf(const Node& node);

 private:
  explicit ArrowFunction(FunctionKind kind);

  DISALLOW_COPY_AND_ASSIGN(ArrowFunction);
};

//
// Class
//
class AOBA_AST_EXPORT Class final : public SyntaxTemplate<Syntax> {
  DECLARE_CONCRETE_AST_SYNTAX(Class, Syntax);

 public:
  ~Class() final;

  static const Node& BodyOf(const Node& node);
  static const Node& HeritageOf(const Node& node);
  static const Node& NameOf(const Node& node);

 private:
  Class();

  DISALLOW_COPY_AND_ASSIGN(Class);
};

//
// Declaration for /** annotation */ MemberExpression|AssignemntExpression
//
class AOBA_AST_EXPORT Declaration final : public Syntax {
  DECLARE_CONCRETE_AST_SYNTAX(Declaration, Syntax);

 public:
  ~Declaration() final;

  static const Node& ExpressionOf(const Node& node);
  static const Node& InitializerOf(const Node& node);

 private:
  Declaration();

  DISALLOW_COPY_AND_ASSIGN(Declaration);
};

//
// Function
//
class AOBA_AST_EXPORT Function final
    : public SyntaxTemplate<Syntax, FunctionKind> {
  DECLARE_CONCRETE_AST_SYNTAX(Function, Syntax);

 public:
  ~Function() final;

  FunctionKind kind() const { return parameter_at<0>(); }

  // Returns |BlockStatement|.
  static const Node& BodyOf(const Node& node);

  // Returns |Name| or |Empty|.
  static const Node& NameOf(const Node& node);

  // Returns |ParameterList| node.
  static const Node& ParametersOf(const Node& node);

 private:
  explicit Function(FunctionKind kind);

  DISALLOW_COPY_AND_ASSIGN(Function);
};

//
// Method
//
class AOBA_AST_EXPORT Method final
    : public SyntaxTemplate<Syntax, MethodKind, FunctionKind> {
  DECLARE_CONCRETE_AST_SYNTAX(Method, Syntax);

 public:
  ~Method() final;

  FunctionKind kind() const { return parameter_at<1>(); }
  MethodKind method_kind() const { return parameter_at<0>(); }

  // Returns |BlockStatement|.
  static const Node& BodyOf(const Node& node);

  static FunctionKind FunctionKindOf(const Node& node);
  static bool IsStatic(const Node& node);
  static MethodKind MethodKindOf(const Node& node);

  // Returns either |ArrayInitializer|, e.g. "[Symbol.foo]" or
  // |ReferenceExpression|.
  static const Node& NameOf(const Node& node);

  // Returns |ParameterList| node.
  static const Node& ParametersOf(const Node& node);

 private:
  Method(MethodKind method_kind, FunctionKind kind);

  DISALLOW_COPY_AND_ASSIGN(Method);
};

}  // namespace ast
}  // namespace aoba

#endif  // AOBA_AST_DECLARATIONS_H_
