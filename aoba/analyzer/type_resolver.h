// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_TYPE_RESOLVER_H_
#define AOBA_ANALYZER_TYPE_RESOLVER_H_

#include <memory>
#include <vector>

#include "aoba/analyzer/pass.h"

#include "aoba/ast/syntax_forward.h"
#include "aoba/ast/syntax_visitor.h"

namespace aoba {
namespace analyzer {

class Annotation;
class Class;
class ClassTreeBuilder;
class Type;
class Value;

//
// TypeResolver
//
class TypeResolver final : public Pass, public ast::SyntaxVisitor {
 public:
  explicit TypeResolver(Context* context);
  ~TypeResolver() final;

  void PrepareForTesting();

 private:
  void SetClassHeritage(const Class& class_value,
                        const Annotation& annotation,
                        const ast::Node& node);
  const Type* ComputeClassType(const ast::Node& node) const;
  const Type* ComputeType(const Annotation& annotation,
                          const ast::Node& node,
                          const Type* maybe_this_type);

  void ProcessAnnotation(const ast::Node& node,
                         const Annotation& annotation,
                         const Type* maybe_this_type);
  void ProcessArrayBinding(const ast::Node& node, const Type& type);

  void ProcessAssignment(const ast::Node& lhs,
                         const ast::Node* maybe_rhs,
                         const Annotation& annotation);

  void ProcessBinding(const ast::Node& node, const Type& type);
  void ProcessClass(const ast::Node& node, const Annotation& annotation);
  void ProcessFunction(const ast::Node& node, const Annotation& annotation);
  void ProcessObjectBinding(const ast::Node& node, const Type& type);

  void ProcessPropertyAssignment(const ast::Node& lhs,
                                 const ast::Node* maybe_rhs,
                                 const Annotation& annotation);

  void ProcessVariableDeclaration(const ast::Node& node,
                                  const Annotation& annotation);
  void RegisterType(const ast::Node& node, const Type& type);

  void RegisterTypeIfPossible(const ast::Node& node,
                              const Annotation& annotation,
                              const Type* maybe_this_type);

  const Class* ResolveClass(const ast::Node& node);
  const Value* SingleValueOf(const ast::Node& node) const;

  // |Pass| members
  void RunOnAll() final;
  void RunOn(const ast::Node& node) final;

  // |ast::SyntaxVisitor| members
  void VisitDefault(const ast::Node& node) final;
  void VisitInternal(const ast::Annotation& syntax,
                     const ast::Node& node) final;

  void VisitInternal(const ast::Class& syntax, const ast::Node& node) final;

  const Class* array_class_ = nullptr;
  const std::unique_ptr<ClassTreeBuilder> class_tree_builder_;
  const Class* object_class_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(TypeResolver);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_TYPE_RESOLVER_H_
