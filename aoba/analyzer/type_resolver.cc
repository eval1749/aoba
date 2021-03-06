// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <utility>

#include "aoba/analyzer/type_resolver.h"

#include "base/logging.h"
#include "aoba/analyzer/annotation_compiler.h"
#include "aoba/analyzer/built_in_world.h"
#include "aoba/analyzer/class_tree_builder.h"
#include "aoba/analyzer/context.h"
#include "aoba/analyzer/error_codes.h"
#include "aoba/analyzer/properties.h"
#include "aoba/analyzer/type_annotation_transformer.h"
#include "aoba/analyzer/type_factory.h"
#include "aoba/analyzer/type_transformer.h"
#include "aoba/analyzer/types.h"
#include "aoba/analyzer/value_editor.h"
#include "aoba/analyzer/values.h"
#include "aoba/ast/bindings.h"
#include "aoba/ast/declarations.h"
#include "aoba/ast/expressions.h"
#include "aoba/ast/jsdoc_syntaxes.h"
#include "aoba/ast/node.h"
#include "aoba/ast/node_traversal.h"
#include "aoba/ast/statements.h"
#include "aoba/ast/types.h"

namespace aoba {
namespace analyzer {

//
// TypeResolver
//
TypeResolver::TypeResolver(Context* context)
    : Pass(context), class_tree_builder_(new ClassTreeBuilder(context)) {}

TypeResolver::~TypeResolver() = default;

// The entry point
void TypeResolver::PrepareForTesting() {
  object_class_ = context().TryClassOf(ast::TokenKind::Object);
  class_tree_builder_->ProcessClassDefinition(*object_class_);
}

void TypeResolver::RunOnAll() {
  class_tree_builder_->Build();
}

void TypeResolver::RunOn(const ast::Node& node) {
  if (!object_class_) {
    object_class_ = context().TryClassOf(ast::TokenKind::Object);
    if (!object_class_) {
      AddError(BuiltInWorld::GetInstance()->NameOf(ast::TokenKind::Object),
               ErrorCode::TYPE_RESOLVER_EXPECT_OBJECT_CLASS);
      object_class_ = &context().InstallClass(ast::TokenKind::Object);
    }
  }
  DCHECK(object_class_);
  if (!array_class_) {
    array_class_ = context().TryClassOf(ast::TokenKind::Array);
    if (!array_class_) {
      AddError(BuiltInWorld::GetInstance()->NameOf(ast::TokenKind::Array),
               ErrorCode::TYPE_RESOLVER_EXPECT_ARRAY_CLASS);
      array_class_ = &context().InstallClass(ast::TokenKind::Array);
    }
  }
  DCHECK(array_class_);
  Visit(node);
}

// private
void TypeResolver::SetClassHeritage(const Class& class_value,
                                    const Annotation& annotation,
                                    const ast::Node& node) {
  const auto is_class = class_value.is_class();
  std::vector<std::pair<const ast::Node*, const Class*>> references;
  std::vector<const Class*> class_list;
  if (node.Is<ast::Class>()) {
    const auto& reference = ast::Class::HeritageOf(node);
    if (!reference.Is<ast::ElisionExpression>()) {
      const auto* referenced_class = ResolveClass(reference);
      if (referenced_class && referenced_class->is_class()) {
        references.emplace_back(&reference, referenced_class);
        class_list.emplace_back(referenced_class);
      } else {
        AddError(reference, ErrorCode::TYPE_RESOLVER_EXPECT_CLASS);
      }
    }
  }

  // Process @extends tags
  for (const auto* extends_tag : annotation.extends_tags()) {
    const auto& reference = extends_tag->child_at(1);
    auto* const referenced_class = ResolveClass(reference);
    if (!referenced_class || is_class != referenced_class->is_class()) {
      AddError(reference, ErrorCode::TYPE_RESOLVER_EXPECT_CLASS);
      continue;
    }
    const auto& it = std::find_if(
        references.begin(), references.end(),
        [&](const std::pair<const ast::Node*, const Class*>& present) {
          return present.second == referenced_class;
        });
    if (it != references.end()) {
      AddError(*extends_tag, ErrorCode::TYPE_RESOLVER_MULTIPLE_OCCURRENCES,
               *it->first);
      continue;
    }
    if (node.Is<ast::Class>()) {
      AddError(*extends_tag, ErrorCode::TYPE_RESOLVER_UNEXPECT_EXTENDS, node);
      continue;
    }
    if (is_class && !class_list.empty()) {
      AddError(*extends_tag, ErrorCode::TYPE_RESOLVER_UNEXPECT_EXTENDS, node);
      continue;
    }
    references.emplace_back(&reference, referenced_class);
    class_list.emplace_back(referenced_class);
  }

  // Install default base class |Object|.
  if (class_value.is_class() && class_list.empty() &&
      class_value != object_class_) {
    references.emplace_back(&object_class_->name(), object_class_);
    class_list.emplace_back(object_class_);
  }

  // Process @implements tags
  for (const auto* implements_tag : annotation.implements_tags()) {
    const auto& reference = implements_tag->child_at(1);
    auto* const referenced_class = ResolveClass(reference);
    if (!referenced_class || referenced_class->is_class()) {
      AddError(*implements_tag, ErrorCode::TYPE_RESOLVER_EXPECT_INTERFACE);
      continue;
    }
    const auto& it = std::find_if(
        references.begin(), references.end(),
        [&](const std::pair<const ast::Node*, const Class*>& present) {
          return present.second == referenced_class;
        });
    if (it != references.end()) {
      AddError(*implements_tag, ErrorCode::TYPE_RESOLVER_MULTIPLE_OCCURRENCES,
               *it->first);
      continue;
    }
    if (!is_class) {
      AddError(*implements_tag, ErrorCode::TYPE_RESOLVER_UNEXPECT_IMPLEMENTS,
               node);
      continue;
    }
    references.emplace_back(&reference, referenced_class);
    class_list.emplace_back(referenced_class);
  }
  DCHECK_EQ(class_list.size(), references.size());
  Value::Editor().SetClassHeritage(class_value, class_list);
  class_tree_builder_->ProcessClassDefinition(class_value);
}

const Type* TypeResolver::ComputeClassType(const ast::Node& node) const {
  if (node.Is<ast::ComputedMemberExpression>()) {
    const auto& member = ast::ComputedMemberExpression::ExpressionOf(node);
    if (!member.Is<ast::MemberExpression>())
      return nullptr;
    if (ast::MemberExpression::NameOf(member) != ast::TokenKind::Prototype)
      return nullptr;
    return context().TryTypeOf(ast::MemberExpression::ContainerOf(member));
  }
  if (node.Is<ast::MemberExpression>()) {
    const auto& member = ast::MemberExpression::ContainerOf(node);
    if (!member.Is<ast::MemberExpression>())
      return nullptr;
    if (ast::MemberExpression::NameOf(member) != ast::TokenKind::Prototype)
      return nullptr;
    return context().TryTypeOf(ast::MemberExpression::ContainerOf(member));
  }
  return nullptr;
}

const Type* TypeResolver::ComputeType(const Annotation& annotation,
                                      const ast::Node& node,
                                      const Type* maybe_this_type) {
  TypeAnnotationTransformer transformer(&context(), annotation, node,
                                        maybe_this_type);
  return transformer.Compile();
}

void TypeResolver::ProcessArrayBinding(const ast::Node& node,
                                       const Type& type) {
  NOTREACHED() << "NYI ProcessArrayBinding" << node << ' ' << type;
}

void TypeResolver::ProcessAnnotation(const ast::Node& node,
                                     const Annotation& annotation,
                                     const Type* maybe_this_type) {
  RegisterTypeIfPossible(node, annotation, maybe_this_type);
}

void TypeResolver::ProcessAssignment(const ast::Node& lhs,
                                     const ast::Node* maybe_rhs,
                                     const Annotation& annotation) {
  auto* const value = SingleValueOf(lhs);
  if (value && value->Is<Class>()) {
    const auto& class_value = value->As<Class>();
    SetClassHeritage(class_value, annotation, lhs);
    const auto& class_type = type_factory().NewClassType(class_value);
    RegisterTypeIfPossible(lhs, annotation, &class_type);
    return;
  }
  RegisterTypeIfPossible(lhs, annotation, nullptr);
}

void TypeResolver::ProcessBinding(const ast::Node& node, const Type& type) {
  if (node.Is<ast::BindingNameElement>())
    return RegisterType(node, type);
  if (node.Is<ast::ArrayBindingPattern>())
    return ProcessArrayBinding(node, type);
  if (node.Is<ast::ObjectBindingPattern>())
    return ProcessObjectBinding(node, type);
  AddError(node, ErrorCode::TYPE_RESOLVER_UNEXPECT_BINDING);
}

void TypeResolver::ProcessClass(const ast::Node& node,
                                const Annotation& annotation) {
  const auto& class_value = context().ValueOf(node).As<Class>();
  SetClassHeritage(class_value, annotation, node);
  const auto& class_type = type_factory().NewClassType(class_value);
  for (const auto& child :
       ast::NodeTraversal::ChildNodesOf(ast::Class::BodyOf(node))) {
    if (!child.Is<ast::Annotation>())
      continue;
    const auto& document = ast::Annotation::DocumentOf(child);
    const auto& member = ast::Annotation::AnnotatedOf(child);
    if (!member.Is<ast::Method>())
      continue;
    const auto annotation = Annotation::Compiler().Compile(document, node);
    const auto* const this_type =
        ast::Method::IsStatic(member) ? nullptr : &class_type;
    ProcessAnnotation(member, annotation, this_type);
  }
}

void TypeResolver::ProcessFunction(const ast::Node& node,
                                   const Annotation& annotation) {
  const auto* const class_value = context().ValueOf(node).TryAs<Class>();
  if (!class_value)
    return ProcessAnnotation(node, annotation, nullptr);
  SetClassHeritage(*class_value, annotation, node);
}

void TypeResolver::ProcessObjectBinding(const ast::Node& node,
                                        const Type& type) {
  NOTREACHED() << "NYI ProcessObjectBinding" << node << ' ' << type;
}

void TypeResolver::ProcessPropertyAssignment(const ast::Node& lhs,
                                             const ast::Node* maybe_rhs,
                                             const Annotation& annotation) {
  DCHECK(IsMemberExpression(lhs));
  ProcessAssignment(lhs, maybe_rhs, annotation);
}

void TypeResolver::ProcessVariableDeclaration(const ast::Node& node,
                                              const Annotation& annotation) {
  for (const auto& binding : ast::NodeTraversal::ChildNodesOf(node)) {
    if (binding.Is<ast::BindingNameElement>()) {
      const auto& initializer = ast::BindingNameElement::InitializerOf(binding);
      ProcessAssignment(binding, &initializer, annotation);
      continue;
    }
    RegisterTypeIfPossible(node, annotation, nullptr);
  }
}

void TypeResolver::RegisterType(const ast::Node& node, const Type& type) {
  context().RegisterType(node, type);
}

void TypeResolver::RegisterTypeIfPossible(const ast::Node& node,
                                          const Annotation& annotation,
                                          const Type* maybe_this_type) {
  const auto* type = ComputeType(annotation, node, maybe_this_type);
  if (!type)
    return;
  RegisterType(node, *type);
}

const Class* TypeResolver::ResolveClass(const ast::Node& node) {
  if (node.Is<ast::TypeName>()) {
    auto* type = context().TryTypeOf(node);
    if (!type || !type->Is<ClassType>())
      return nullptr;
    return &type->As<ClassType>().value();
  }
  if (node.Is<ast::TypeApplication>()) {
    const auto& type =
        TypeTransformer(&context()).TransformTypeApplication(node);
    if (!type.Is<ClassType>())
      return nullptr;
    return &type.As<ClassType>().value();
  }
  auto* value = context().TryValueOf(node);
  if (!value)
    return nullptr;
  if (value->Is<Class>())
    return &value->As<Class>();
  if (!value->Is<Variable>())
    return nullptr;
  const auto& variable = value->As<Variable>();
  if (variable.assignments().size() != 1)
    return nullptr;
  const auto& assignment_value = variable.assignments().front();
  return assignment_value.TryAs<Class>();
}

// Returns value associated to |node|.
const Value* TypeResolver::SingleValueOf(const ast::Node& node) const {
  DCHECK(IsMemberExpression(node) || node.Is<ast::BindingNameElement>())
      << node;
  auto* value = context().TryValueOf(node);
  if (!value || !value->Is<ValueHolder>())
    return nullptr;
  const auto& holder = value->As<ValueHolder>();
  if (holder.assignments().size() != 1)
    return nullptr;
  return &holder.assignments().front();
}

// |ast::SyntaxVisitor| members
void TypeResolver::VisitDefault(const ast::Node& node) {
  for (const ast::Node& child : ast::NodeTraversal::ChildNodesOf(node))
    Visit(child);
}

void TypeResolver::VisitInternal(const ast::Annotation& syntax,
                                 const ast::Node& node) {
  const auto& annotated = ast::Annotation::AnnotatedOf(node);
  const auto& document = ast::Annotation::DocumentOf(node);
  const auto annotation = Annotation::Compiler().Compile(document, node);
  if (annotated.Is<ast::Class>())
    return ProcessClass(annotated, annotation);
  if (annotated.Is<ast::Function>())
    return ProcessFunction(annotated, annotation);
  if (annotated.Is<ast::GroupExpression>())
    return ProcessAnnotation(annotated, annotation, nullptr);
  if (annotated.syntax().Is<ast::VariableDeclaration>())
    return ProcessVariableDeclaration(annotated, annotation);
  if (!annotated.Is<ast::ExpressionStatement>()) {
    Visit(annotated);
    return;
  }
  const auto& expression = ast::ExpressionStatement::ExpressionOf(annotated);
  if (expression.Is<ast::AssignmentExpression>()) {
    const auto& lhs = ast::AssignmentExpression::LeftHandSideOf(expression);
    const auto& rhs = ast::AssignmentExpression::RightHandSideOf(expression);
    if (!IsMemberExpression(lhs))
      return;
    ProcessPropertyAssignment(lhs, &rhs, annotation);
  }
  if (IsMemberExpression(expression))
    return ProcessPropertyAssignment(expression, nullptr, annotation);
  const auto* const class_type = ComputeClassType(expression);
  return ProcessAnnotation(expression, annotation, class_type);
}

void TypeResolver::VisitInternal(const ast::Class& syntax,
                                 const ast::Node& node) {
  ProcessClass(node, Annotation());
}

}  // namespace analyzer
}  // namespace aoba
