// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_AST_COMPILATION_UNITS_H_
#define AOBA_AST_COMPILATION_UNITS_H_

#include <unordered_map>

#include "aoba/ast/syntax.h"
#include "aoba/base/memory/zone_unordered_map.h"

namespace aoba {
namespace ast {

//
// CompilationUnit
//
class AOBA_AST_EXPORT CompilationUnit : public SyntaxTemplate<Syntax> {
  DECLARE_ABSTRACT_AST_SYNTAX(CompilationUnit, Syntax);

 public:
  ~CompilationUnit() override;

 protected:
  explicit CompilationUnit(SyntaxCode syntax_code);

 private:
  DISALLOW_COPY_AND_ASSIGN(CompilationUnit);
};

//
// Externs
//
class AOBA_AST_EXPORT Externs final : public CompilationUnit {
  DECLARE_CONCRETE_AST_SYNTAX(Externs, CompilationUnit);

 public:
  ~Externs() final;

 private:
  Externs();

  DISALLOW_COPY_AND_ASSIGN(Externs);
};

//
// Module
//
class AOBA_AST_EXPORT Module final : public CompilationUnit {
  DECLARE_CONCRETE_AST_SYNTAX(Module, CompilationUnit);

 public:
  ~Module() final;

 private:
  Module();

  DISALLOW_COPY_AND_ASSIGN(Module);
};

//
// Script
//
class AOBA_AST_EXPORT Script final : public CompilationUnit {
  DECLARE_CONCRETE_AST_SYNTAX(Script, CompilationUnit);

 public:
  ~Script() final;

 private:
  Script();

  DISALLOW_COPY_AND_ASSIGN(Script);
};

}  // namespace ast
}  // namespace aoba

#endif  // AOBA_AST_COMPILATION_UNITS_H_
