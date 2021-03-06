// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iterator>

#include "aoba/analyzer/context.h"

#include "aoba/analyzer/built_in_world.h"
#include "aoba/analyzer/factory.h"
#include "aoba/analyzer/properties_editor.h"
#include "aoba/analyzer/public/analyzer_settings.h"
#include "aoba/analyzer/type_factory.h"
#include "aoba/analyzer/type_map.h"
#include "aoba/analyzer/types.h"
#include "aoba/analyzer/value_editor.h"
#include "aoba/analyzer/value_map.h"
#include "aoba/analyzer/values.h"
#include "aoba/ast/compilation_units.h"
#include "aoba/ast/node.h"
#include "aoba/ast/tokens.h"
#include "aoba/base/error_sink.h"
#include "aoba/base/memory/zone.h"
#include "aoba/base/source_code.h"
#include "aoba/base/source_code_range.h"

namespace aoba {
namespace analyzer {

//
// Context
//
Context::Context(const AnalyzerSettings& settings)
    : factory_(new Factory(&settings.zone())),
      global_properties_(factory_->NewProperties(
          BuiltInWorld::GetInstance()->NameOf(ast::TokenKind::Global))),
      settings_(settings),
      type_factory_(new TypeFactory(&settings.zone())),
      type_map_(new TypeMap()),
      value_map_(new ValueMap()) {}

Context::~Context() = default;

ErrorSink& Context::error_sink() const {
  return settings_.error_sink();
}

Zone& Context::zone() const {
  return settings_.zone();
}

void Context::AddError(const ast::Node& node,
                       ErrorCode error_code,
                       const ast::Node& related) {
  if (node.range().source_code() == related.range().source_code()) {
    AddError(SourceCodeRange::Merge(node.range(), related.range()), error_code);
    return;
  }
  AddError(related, error_code);
  AddError(node, error_code);
}

void Context::AddError(const ast::Node& node, ErrorCode error_code) {
  AddError(node.range(), error_code);
}

void Context::AddError(const SourceCodeRange& range, ErrorCode error_code) {
  error_sink().AddError(range, error_code);
}

void Context::RegisterValue(const ast::Node& node, const Value& value) {
  value_map_->RegisterValue(node, value);
}

const Value* Context::TryValueOf(const ast::Node& node) const {
  return value_map_->TryValueOf(node);
}

const Value& Context::ValueOf(const ast::Node& node) const {
  return value_map_->ValueOf(node);
}

void Context::RegisterType(const ast::Node& node, const Type& type) {
  type_map_->RegisterType(node, type);
}

const Type* Context::TryTypeOf(const ast::Node& node) const {
  return type_map_->TryTypeOf(node);
}

const Type& Context::TypeOf(const ast::Node& node) const {
  return type_map_->TypeOf(node);
}

// Global class
const Class& Context::InstallClass(ast::TokenKind name_id) {
  const auto& class_name = BuiltInWorld::GetInstance()->NameOf(name_id);
  auto& properties = factory().NewProperties(class_name);
  const auto& object_class =
      name_id == ast::TokenKind::Array
          ? factory().NewGenericClass(
                ClassKind::Class, class_name, class_name,
                {&type_factory()
                      .NewTypeParameter(BuiltInWorld::GetInstance()->NameOf(
                          ast::TokenKind::Object))
                      .As<TypeParameter>()},
                &properties)
          : factory().NewNormalClass(ClassKind::Class, class_name, class_name,
                                     &properties);
  const auto& object_variable = factory().NewVariable(
      VariableKind::Const, class_name, &factory().NewValueHolderData(),
      &factory().NewProperties(class_name));
  const auto& object_property = factory().NewProperty(
      Visibility::Public, class_name, &object_variable.data(),
      &object_variable.properties());
  RegisterValue(class_name, object_class);
  Value::Editor().AddAssignment(object_property, object_class);
  Properties::Editor().Add(&global_properties(), object_property);
  return object_class;
}

const Class* Context::TryClassOf(ast::TokenKind name_id) const {
  const auto& object_name = BuiltInWorld::GetInstance()->NameOf(name_id);
  auto* object_property = global_properties().TryGet(object_name);
  if (object_property->assignments().size() != 1)
    return nullptr;
  const auto& object_value = object_property->assignments().front();
  return object_value.TryAs<Class>();
}

void Context::ResetCurrentIdForTesting(int current_id) {
  factory().ResetCurrentIdForTesting(current_id);
  type_factory().ResetCurrentIdForTesting(current_id);
}

}  // namespace analyzer
}  // namespace aoba
