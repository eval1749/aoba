// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <vector>

#include "aoba/analyzer/type_transformer.h"

#include "aoba/analyzer/context.h"
#include "aoba/analyzer/error_codes.h"
#include "aoba/analyzer/factory.h"
#include "aoba/analyzer/type_factory.h"
#include "aoba/analyzer/types.h"
#include "aoba/analyzer/values.h"
#include "aoba/ast/expressions.h"
#include "aoba/ast/node_traversal.h"
#include "aoba/ast/types.h"

namespace aoba {
namespace analyzer {

namespace {

bool CanBeNullable(const Type& type) {
  if (type.Is<AnyType>())
    return false;
  if (type.Is<LabeledType>()) {
    VLOG(0) << "NYI : " << type;
    return false;
  }
  if (type.Is<NilType>())
    return false;
  if (type.Is<NullType>())
    return false;
  if (type.Is<TypeAlias>()) {
    VLOG(0) << "NYI : " << type;
    return false;
  }
  if (type.Is<UnspecifiedType>())
    return false;
  if (type.Is<VoidType>())
    return false;
  return true;
}

const ast::Node* FindName(const ast::Node& name,
                          const std::vector<const LabeledType*>& members) {
  const auto name_id = ast::Name::KindOf(name);
  const auto& it = std::find_if(
      members.begin(), members.end(), [&](const LabeledType* type) {
        return ast::Name::KindOf(type->name()) == name_id;
      });
  return it == members.end() ? nullptr : &(*it)->name();
}

}  // namespace

//
// TypeTransformer
//
TypeTransformer::TypeTransformer(Context* context) : ContextUser(context) {}
TypeTransformer::~TypeTransformer() = default;

const Type& TypeTransformer::NewNullableType(const Type& type) {
  if (type.Is<AnyType>() || type.Is<InvalidType>() ||
      type.Is<UnspecifiedType>()) {
    return type;
  }
  // TODO(eval1749): We should expand |TypeAlia| in another place.
  if (!type.Is<TypeAlias>())
    DCHECK(CanBeNullable(type)) << type;
  return type_factory().NewUnionType(type, null_type());
}

const Type& TypeTransformer::Transform(const ast::Node& node) {
  DCHECK(node.syntax().Is<ast::Type>()) << node;
  if (node.Is<ast::AnyType>())
    return any_type();
  if (node.Is<ast::FunctionType>())
    return TransformFunctionType(node);
  if (node.Is<ast::InvalidType>())
    return unspecified_type();
  if (node.Is<ast::NonNullableType>())
    return TransformNonNullableType(node);
  if (node.Is<ast::NullableType>())
    return NewNullableType(Transform(node.child_at(0)));
  if (node.Is<ast::OptionalType>()) {
    AddError(node, ErrorCode::JSDOC_UNEXPECT_OPTIONAL);
    return Transform(node.child_at(0));
  }
  if (node.Is<ast::RestType>()) {
    AddError(node, ErrorCode::JSDOC_UNEXPECT_REST);
    return Transform(node.child_at(0));
  }
  if (node.Is<ast::RecordType>())
    return TransformRecordType(node);
  if (node.Is<ast::TupleType>()) {
    std::vector<const Type*> types;
    for (const auto& member : ast::NodeTraversal::ChildNodesOf(node))
      types.push_back(&Transform(member));
    return type_factory().NewTupleTypeFromVector(types);
  }
  if (node.Is<ast::TypeApplication>()) {
    const auto& type = TransformTypeApplication(node);
    if (!type.Is<ClassType>())
      return type;
    return NewNullableType(type);
  }
  if (node.Is<ast::TypeName>())
    return TransformTypeName(node);
  if (node.Is<ast::TypeGroup>())
    return Transform(ast::TypeGroup::TypeOf(node));
  if (node.Is<ast::UnionType>()) {
    std::vector<const Type*> types;
    for (const auto& member : ast::NodeTraversal::ChildNodesOf(node))
      types.push_back(&Transform(member));
    return type_factory().NewUnionTypeFromVector(types);
  }
  if (node.Is<ast::UnknownType>()) {
    // Unknown type is the source of bug, we should avoid to use.
    return any_type();
  }
  if (node.Is<ast::VoidType>())
    return void_type();
  DVLOG(0) << "We should handle " << node;
  return unspecified_type();
}

const Type& TypeTransformer::TransformFunctionType(const ast::Node& node) {
  DCHECK_EQ(node, ast::SyntaxCode::FunctionType);
  FunctionTypeArity arity;
  std::vector<const Type*> parameter_types;
  const Type* this_type = &unspecified_type();
  const auto kind = ast::FunctionType::KindOf(node);
  enum class State {
    Optional,
    Required,
    Rest,
    This,
  } state = kind == ast::FunctionTypeKind::This ? State::This : State::Required;
  for (const auto& parameter : ast::NodeTraversal::ChildNodesOf(
           ast::FunctionType::ParameterTypesOf(node))) {
    const auto is_optional = parameter.Is<ast::OptionalType>();
    const auto is_rest = parameter.Is<ast::RestType>();
    const auto& type =
        Transform(is_optional || is_rest ? parameter.child_at(0) : parameter);
    switch (state) {
      case State::Optional:
        parameter_types.push_back(&type);
        if (is_rest) {
          state = State::Rest;
          continue;
        }
        ++arity.maximum;
        continue;
      case State::Required:
        parameter_types.push_back(&type);
        if (is_rest) {
          state = State::Rest;
          continue;
        }
        ++arity.maximum;
        if (is_optional) {
          state = State::Optional;
          continue;
        }
        arity.minimum = arity.maximum;
        continue;
      case State::Rest:
        AddError(node, ErrorCode::JSDOC_UNEXPECT_REST);
        continue;
      case State::This:
        this_type = &type;
        state = State::Required;
        continue;
    }
    VLOG(0) << "NYI : "
            << "Invalid state " << static_cast<int>(state) << parameter;
  }
  DCHECK_NE(state, State::This);
  return type_factory().NewFunctionType(
      kind == ast::FunctionTypeKind::New ? FunctionTypeKind::Constructor
                                         : FunctionTypeKind::Normal,
      std::vector<const TypeParameter*>(), arity, parameter_types,
      Transform(ast::FunctionType::ReturnTypeOf(node)), *this_type);
}

const Type& TypeTransformer::TransformNonNullableType(const ast::Node& node) {
  DCHECK_EQ(node, ast::SyntaxCode::NonNullableType);
  const auto& type_node = ast::NonNullableType::TypeOf(node);
  const auto& type = Transform(type_node);
  if (!type.Is<UnionType>()) {
    // TODO(eval1749): We should support "!*" == any type except for null type.
    AddError(type_node, ErrorCode::JSDOC_EXPECT_NULLABLE_TYPE);
    return type;
  }
  // Remove null type from members in union type.
  const auto& union_type = type.As<UnionType>();
  std::vector<const Type*> members;
  for (const auto& member : union_type.members()) {
    if (member.Is<NullType>())
      continue;
    members.push_back(&member);
  }
  if (members.size() == union_type.members().size()) {
    AddError(type_node, ErrorCode::JSDOC_EXPECT_NULLABLE_TYPE);
    return type;
  }
  return type_factory().NewUnionTypeFromVector(members);
}

const Type& TypeTransformer::TransformRecordType(const ast::Node& node) {
  DCHECK_EQ(node, ast::SyntaxCode::RecordType);
  std::vector<const LabeledType*> members;
  for (const auto& member : ast::NodeTraversal::ChildNodesOf(node)) {
    if (member.Is<ast::Name>()) {
      const auto& name = member;
      if (const auto* present = FindName(name, members)) {
        AddError(name, ErrorCode::JSDOC_MULTIPLE_PROPERTY, *present);
        continue;
      }
      members.push_back(
          &type_factory().NewLabeledType(name, any_type()).As<LabeledType>());
      continue;
    }
    if (!member.Is<ast::Property>()) {
      AddError(member, ErrorCode::JSDOC_EXPECT_PROPERTY);
      continue;
    }
    const auto& name = ast::Property::NameOf(member);
    if (const auto* present = FindName(name, members)) {
      AddError(name, ErrorCode::JSDOC_MULTIPLE_PROPERTY, *present);
      continue;
    }
    const auto& type = Transform(ast::Property::ValueOf(member));
    members.push_back(
        &type_factory().NewLabeledType(name, type).As<LabeledType>());
  }
  if (members.empty())
    AddError(node, ErrorCode::JSDOC_EMPTY_RECORD_TYPE);
  return NewNullableType(type_factory().NewRecordType(members));
}

const Type& TypeTransformer::TransformTypeApplication(const ast::Node& node) {
  const auto& generic_type_name = ast::TypeApplication::NameOf(node);
  const auto* maybe_generic_type = context().TryTypeOf(generic_type_name);
  if (!maybe_generic_type) {
    AddError(generic_type_name, ErrorCode::JSDOC_EXPECT_GENERIC_CLASS);
    return unspecified_type();
  }
  const auto& generic_type = *maybe_generic_type;
  if (!generic_type.Is<ClassType>()) {
    AddError(generic_type_name, ErrorCode::JSDOC_EXPECT_GENERIC_CLASS);
    return unspecified_type();
  }
  auto* const generic_class_value =
      generic_type.As<ClassType>().value().TryAs<GenericClass>();
  if (!generic_class_value) {
    AddError(generic_type_name, ErrorCode::JSDOC_EXPECT_GENERIC_CLASS);
    return unspecified_type();
  }
  const auto& arguments_node = ast::TypeApplication::ArgumentsOf(node);
  if (arguments_node.arity() != generic_class_value->parameters().size()) {
    AddError(node, ErrorCode::JSDOC_INVALID_ARGUMENTS);
    return unspecified_type();
  }
  std::vector<const Type*> arguments;
  for (const auto& argument_node :
       ast::NodeTraversal::ChildNodesOf(arguments_node))
    arguments.push_back(&Transform(argument_node));
  const auto& value =
      factory().NewConstructedClass(*generic_class_value, arguments);
  return type_factory().NewClassType(value);
}

const Type& TypeTransformer::TransformTypeName(const ast::Node& node) {
  DCHECK_EQ(node, ast::SyntaxCode::TypeName);
  const auto* maybe_type = context().TryTypeOf(node);
  if (!maybe_type) {
    // TODO(eval1749): How do we handle undefined type?
    AddError(node, ErrorCode::JSDOC_UNDEFINED_TYPE);
    return unspecified_type();
  }
  const auto& type = *maybe_type;
  if (type.Is<ClassType>() || type.Is<FunctionType>())
    return NewNullableType(type);
  return type;
}

}  // namespace analyzer
}  // namespace aoba
