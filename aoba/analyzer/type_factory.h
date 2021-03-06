// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_TYPE_FACTORY_H_
#define AOBA_ANALYZER_TYPE_FACTORY_H_

#include <memory>
#include <vector>

#include "base/macros.h"

namespace aoba {

namespace ast {
class Node;
enum class TokenKind;
}

class Zone;

namespace analyzer {

class Class;
struct FunctionTypeArity;
enum class FunctionTypeKind;
class LabeledType;
class Type;
class TypeParameter;
class Value;
class Variable;

//
// TypeFactory
//
class TypeFactory final {
 public:
  explicit TypeFactory(Zone* zone);
  ~TypeFactory();

  const Type& any_type() { return any_type_; }
  const Type& invalid_type() const { return invalid_type_; }
  const Type& nil_type() { return nil_type_; }
  const Type& null_type() const { return null_type_; }
  const Type& unspecified_type() const { return unspecified_type_; }
  const Type& void_type() const { return void_type_; }

  const Type& NewClassType(const Class& class_value);
  const Type& NewFunctionType(
      FunctionTypeKind kind,
      const std::vector<const TypeParameter*>& type_parameters,
      const FunctionTypeArity& arity,
      const std::vector<const Type*>& parameters,
      const Type& return_type,
      const Type& this_type);
  const Type& NewLabeledType(const ast::Node& name, const Type& type);
  const Type& NewPrimitiveType(const ast::TokenKind id);
  const Type& NewRecordType(const std::vector<const LabeledType*>& members);
  const Type& NewTupleTypeFromVector(const std::vector<const Type*>& members);
  const Type& NewTypeAlias(const ast::Node& name, const ast::Node& type);
  const Type& NewTypeName(const ast::Node& name);
  const Type& NewTypeParameter(const ast::Node& name);
  const Type& NewUnionTypeFromVector(const std::vector<const Type*>& members);

  template <typename... Parameters>
  const Type& NewTupleType(const Parameters&... members) {
    return NewTupleTypeFromVector({&members...});
  }

  template <typename... Parameters>
  const Type& NewUnionType(const Parameters&... members) {
    return NewUnionTypeFromVector({&members...});
  }

  void ResetCurrentId();
  void ResetCurrentIdForTesting(int next_id);

 private:
  class Cache;

  void InstallPrimitiveTypes();
  int NextTypeId();

  std::unique_ptr<Cache> cache_;
  int current_type_id_ = 0;
  Zone& zone_;

  const Type& any_type_;
  const Type& invalid_type_;
  const Type& nil_type_;
  const Type& null_type_;
  const Type& unspecified_type_;
  const Type& void_type_;

  DISALLOW_COPY_AND_ASSIGN(TypeFactory);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_TYPE_FACTORY_H_
