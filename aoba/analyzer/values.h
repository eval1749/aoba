// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_VALUES_H_
#define AOBA_ANALYZER_VALUES_H_

#include <iosfwd>
#include <vector>

#include "aoba/analyzer/value.h"
#include "aoba/base/block_range.h"
#include "aoba/base/iterator_utils.h"
#include "aoba/base/memory/zone_vector.h"

namespace aoba {
namespace analyzer {

class Function;
class GenericClass;
class Properties;
class Type;
class TypeParameter;

//
// ClassKind
//
enum class ClassKind {
  Class,
  Interface,
  Record,
};

std::ostream& operator<<(std::ostream& ostream, ClassKind kind);

//
// VariableKind
//
#define FOR_EACH_VARIABLE_KIND(V) \
  V(Catch)                        \
  V(Class)                        \
  V(Const)                        \
  V(Function)                     \
  V(Let)                          \
  V(Parameter)                    \
  V(Var)

enum class VariableKind {
#define V(name) name,
  FOR_EACH_VARIABLE_KIND(V)
#undef V
};

std::ostream& operator<<(std::ostream& ostream, VariableKind kind);

//
// Visibility
//
#define FOR_EACH_VISIBILITY(V) \
  V(Private)                   \
  V(Protected)                 \
  V(Public)

enum class Visibility {
#define V(name) name,
  FOR_EACH_VISIBILITY(V)
#undef V
};

std::ostream& operator<<(std::ostream& ostream, Visibility visibility);

//
// Object
//
class Object : public Value {
  DECLARE_ABSTRACT_ANALYZE_VALUE(Object, Value);

 public:
  ~Object() override;

  Properties& properties() const { return properties_; }

 protected:
  Object(int id, const ast::Node& node, Properties* properties);

 private:
  Properties& properties_;

  DISALLOW_COPY_AND_ASSIGN(Object);
};

//
// Class
//
class Class : public Object {
  DECLARE_ABSTRACT_ANALYZE_VALUE(Class, Object);

 public:
  ~Class() override;

  auto base_classes() const { return ReferenceRangeOf(base_classes_); }
  internal::ReferenceRange<ZoneVector<const Class*>::const_iterator>
  class_list() const;
  bool is_class() const { return kind_ == ClassKind::Class; }
  bool is_finalized() const { return !class_list_.empty(); }
  ClassKind kind() const { return kind_; }
  const ast::Node& name() const { return name_; }
  Properties& prototype_properties() const;

 protected:
  Class(Zone* zone,
        int id,
        ClassKind kind,
        const ast::Node& name,
        const ast::Node& node,
        Properties* properties);

 private:
  ZoneVector<const Class*> base_classes_;
  ZoneVector<const Class*> class_list_;
  const ClassKind kind_;
  const ast::Node& name_;

  DISALLOW_COPY_AND_ASSIGN(Class);
};

//
// ValueHolderData
//
class ValueHolderData final : public ZoneAllocated {
 public:
  ~ValueHolderData();

  auto assignments() const { return ReferenceRangeOf(assignments_); }
  auto references() const { return ReferenceRangeOf(references_); }

 private:
  friend class Factory;
  friend class Value::Editor;

  explicit ValueHolderData(Zone* zone);

  ZoneVector<const Value*> assignments_;
  ZoneVector<const ast::Node*> references_;

  DISALLOW_COPY_AND_ASSIGN(ValueHolderData);
};

//
// ValueHolder
//
class ValueHolder : public Object {
  DECLARE_ABSTRACT_ANALYZE_VALUE(ValueHolder, Object);

 public:
  ~ValueHolder() override;

  auto assignments() const { return data_.assignments(); }
  ValueHolderData& data() const { return data_; }
  auto references() const { return data_.references(); }

 protected:
  ValueHolder(int id,
              const ast::Node& node,
              ValueHolderData* data,
              Properties* properties);

 private:
  ValueHolderData& data_;

  DISALLOW_COPY_AND_ASSIGN(ValueHolder);
};

//
// ConstructedClass
//
class ConstructedClass : public Class {
  DECLARE_CONCRETE_ANALYZE_VALUE(ConstructedClass, Class);

 public:
  void* operator new(size_t size, void* pointer) { return pointer; }

  ~ConstructedClass() final;

  BlockRange<const Type*> arguments() const;
  const GenericClass& generic_class() const { return generic_class_; }

 private:
  ConstructedClass(Zone* zone,
                   int id,
                   const GenericClass& generic_class,
                   const std::vector<const Type*>& arguments);

  const GenericClass& generic_class_;
  const size_t number_of_arguments_;
  const Type* arguments_[1];

  DISALLOW_COPY_AND_ASSIGN(ConstructedClass);
};

//
// Function
//
class Function final : public Object {
  DECLARE_CONCRETE_ANALYZE_VALUE(Function, Value)

 public:
  ~Function() final;

  const ast::Node& name() const { return name_; }

 private:
  // |properties| is used for holding members of anonymous class
  Function(int id,
           const ast::Node& name,
           const ast::Node& node,
           Properties* properties);

  const ast::Node& name_;

  DISALLOW_COPY_AND_ASSIGN(Function);
};

//
// GenericClass
//
class GenericClass : public Class {
  DECLARE_CONCRETE_ANALYZE_VALUE(GenericClass, Class);

 public:
  void* operator new(size_t size, void* pointer) { return pointer; }

  ~GenericClass() final;

  BlockRange<const TypeParameter*> parameters() const;

 private:
  GenericClass(Zone* zone,
               int id,
               ClassKind kind,
               const ast::Node& name,
               const ast::Node& node,
               const std::vector<const TypeParameter*>& parameters,
               Properties* properties);

 private:
  const size_t number_of_parameters_;
  const TypeParameter* parameters_[1];

  DISALLOW_COPY_AND_ASSIGN(GenericClass);
};

//
// NormalClass
//
class NormalClass : public Class {
  DECLARE_CONCRETE_ANALYZE_VALUE(NormalClass, Class);

 public:
  ~NormalClass() final;

 private:
  NormalClass(Zone* zone,
              int id,
              ClassKind kind,
              const ast::Node& name,
              const ast::Node& node,
              Properties* properties);

  DISALLOW_COPY_AND_ASSIGN(NormalClass);
};

//
// OrdinaryObject
//
class OrdinaryObject final : public Object {
  DECLARE_CONCRETE_ANALYZE_VALUE(OrdinaryObject, Object)

 public:
  ~OrdinaryObject() final;

 private:
  OrdinaryObject(int id, const ast::Node& node, Properties* properties);

  DISALLOW_COPY_AND_ASSIGN(OrdinaryObject);
};

//
// Property
//
class Property final : public ValueHolder {
  DECLARE_CONCRETE_ANALYZE_VALUE(Property, ValueHolder)

 public:
  ~Property() final;

  const ast::Node& key() const { return node(); }
  Visibility visibility() const { return visibility_; }

 private:
  Property(int id,
           Visibility visibility,
           const ast::Node& key,
           ValueHolderData* data,
           Properties* properties);

  const Visibility visibility_;

  DISALLOW_COPY_AND_ASSIGN(Property);
};

//
// Undefined
//
class Undefined final : public Value {
  DECLARE_CONCRETE_ANALYZE_VALUE(Undefined, Value)

 public:
  ~Undefined() final;

 private:
  Undefined(int id, const ast::Node& node);

  DISALLOW_COPY_AND_ASSIGN(Undefined);
};

//
// Variable
//
class Variable final : public ValueHolder {
  DECLARE_CONCRETE_ANALYZE_VALUE(Variable, ValueHolder)

 public:
  ~Variable() final;

  VariableKind kind() const { return kind_; }

 private:
  // |name| should be |ast::Name| or |ast::BindingNameElement|.
  Variable(int id,
           VariableKind kind,
           const ast::Node& name,
           ValueHolderData* data,
           Properties* properties);

  const VariableKind kind_;

  DISALLOW_COPY_AND_ASSIGN(Variable);
};

bool CanBeValueName(const ast::Node& node);

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_VALUES_H_
