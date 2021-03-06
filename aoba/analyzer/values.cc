// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/analyzer/values.h"

#include "aoba/analyzer/built_in_world.h"
#include "aoba/analyzer/properties.h"
#include "aoba/ast/bindings.h"
#include "aoba/ast/expressions.h"
#include "aoba/ast/syntax_forward.h"
#include "aoba/ast/tokens.h"

namespace aoba {
namespace analyzer {

//
// Class
//
Class::Class(Zone* zone,
             int id,
             ClassKind kind,
             const ast::Node& name,
             const ast::Node& node,
             Properties* properties)
    : Object(id, node, properties),
      base_classes_(zone),
      class_list_(zone),
      kind_(kind),
      name_(name) {
  DCHECK(CanBeValueName(name)) << name;
}

Class::~Class() = default;

internal::ReferenceRange<ZoneVector<const Class*>::const_iterator>
Class::class_list() const {
  DCHECK(is_finalized()) << *this;
  return ReferenceRangeOf(class_list_);
}

Properties& Class::prototype_properties() const {
  return properties()
      .Get(BuiltInWorld::GetInstance()->NameOf(ast::TokenKind::Prototype))
      .properties();
}

//
// ConstructedClass
//
ConstructedClass::ConstructedClass(Zone* zone,
                                   int id,
                                   const GenericClass& generic_class,
                                   const std::vector<const Type*>& arguments)
    : Class(zone,
            id,
            generic_class.kind(),
            generic_class.name(),
            generic_class.node(),
            &generic_class.properties()),
      generic_class_(generic_class),
      number_of_arguments_(arguments.size()) {
  DCHECK_EQ(arguments.size(), generic_class.parameters().size());
  DCHECK_GE(arguments.size(), static_cast<size_t>(1));
  auto** runner = arguments_;
  for (auto* argument : arguments) {
    *runner = argument;
    ++runner;
  }
}

ConstructedClass::~ConstructedClass() = default;

BlockRange<const Type*> ConstructedClass::arguments() const {
  return BlockRange<const Type*>(arguments_, number_of_arguments_);
}

//
// Function
//
Function::Function(int id,
                   const ast::Node& name,
                   const ast::Node& node,
                   Properties* properties)
    : Object(id, node, properties), name_(name) {
  DCHECK(CanBeValueName(name)) << name;
}

Function::~Function() = default;

//
// GenericClass
//
GenericClass::GenericClass(Zone* zone,
                           int id,
                           ClassKind kind,
                           const ast::Node& name,
                           const ast::Node& node,
                           const std::vector<const TypeParameter*>& parameters,
                           Properties* properties)
    : Class(zone, id, kind, name, node, properties),
      number_of_parameters_(parameters.size()) {
  DCHECK_GE(parameters.size(), static_cast<size_t>(1));
  auto** runner = parameters_;
  for (auto* parameter : parameters) {
    *runner = parameter;
    ++runner;
  }
}

GenericClass::~GenericClass() = default;

BlockRange<const TypeParameter*> GenericClass::parameters() const {
  return BlockRange<const TypeParameter*>(parameters_, number_of_parameters_);
}

//
// NormalClass
//
NormalClass::NormalClass(Zone* zone,
                         int id,
                         ClassKind kind,
                         const ast::Node& name,
                         const ast::Node& node,
                         Properties* properties)
    : Class(zone, id, kind, name, node, properties) {}

NormalClass::~NormalClass() = default;

//
// Object
//
Object::Object(int id, const ast::Node& node, Properties* properties)
    : Value(id, node), properties_(*properties) {}

Object::~Object() = default;

//
// OrdinaryObject
//
OrdinaryObject::OrdinaryObject(int id,
                               const ast::Node& node,
                               Properties* properties)
    : Object(id, node, properties) {}

OrdinaryObject::~OrdinaryObject() = default;

//
// Property
//
Property::Property(int id,
                   Visibility visibility,
                   const ast::Node& key,
                   ValueHolderData* data,
                   Properties* properties)
    : ValueHolder(id, key, data, properties), visibility_(visibility) {
  DCHECK(CanBeValueName(key)) << key;
}

Property::~Property() = default;

//
// Undefined
//
Undefined::Undefined(int id, const ast::Node& node) : Value(id, node) {}
Undefined::~Undefined() = default;

//
// ValueHolder
//
ValueHolder::ValueHolder(int id,
                         const ast::Node& node,
                         ValueHolderData* data,
                         Properties* properties)
    : Object(id, node, properties), data_(*data) {
  DCHECK(data);
}

ValueHolder::~ValueHolder() = default;

//
// ValueHolderData
//
ValueHolderData::ValueHolderData(Zone* zone)
    : assignments_(zone), references_(zone) {}

ValueHolderData::~ValueHolderData() = default;

//
// Variable
//
Variable::Variable(int id,
                   VariableKind kind,
                   const ast::Node& name,
                   ValueHolderData* data,
                   Properties* properties)
    : ValueHolder(id, name, data, properties), kind_(kind) {
  DCHECK_EQ(name, ast::SyntaxCode::Name);
}

Variable::~Variable() = default;

// Returns true if |node| can be value name.
// Example:
//  var unique = { 'foo': 1 }
bool CanBeValueName(const ast::Node& node) {
  return node.Is<ast::Name>() || node.Is<ast::Empty>() ||
         node.syntax().Is<ast::Expression>();
}

}  // namespace analyzer
}  // namespace aoba
