// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iterator>
#include <type_traits>
#include <vector>

#ifndef AOBA_IR_COMPOSITE_TYPES_H_
#define AOBA_IR_COMPOSITE_TYPES_H_

#include <tuple>

#include "aoba/base/iterator_utils.h"
#include "aoba/base/memory/zone_vector.h"
#include "aoba/ir/type.h"

namespace aoba {
namespace ir {

//
// CompositeType
//
class AOBA_IR_EXPORT CompositeType : public Type {
  DECLARE_ABSTRACT_IR_TYPE(CompositeType, Type);

 public:
  ~CompositeType() override;

 protected:
  CompositeType();

 private:
  DISALLOW_COPY_AND_ASSIGN(CompositeType);
};

//
// CompositeTypeTemplate
//
template <typename Base, typename... Members>
class CompositeTypeTemplate : public Base {
  static_assert(sizeof...(Members) >= 1,
                "CompositeTypeTemplate should have at least one member.");

 public:
  ~CompositeTypeTemplate() override = default;

 protected:
  template <typename... Parameters>
  CompositeTypeTemplate(const std::tuple<Members...> members,
                        Parameters... parameters)
      : Base(parameters...), members_(members) {}

  template <size_t kIndex>
  auto member_at() const {
    return std::get<kIndex>(members_);
  }

 private:
  std::tuple<Members...> members_;

  DISALLOW_COPY_AND_ASSIGN(CompositeTypeTemplate);
};

//
// FunctionType
//
class AOBA_IR_EXPORT FunctionType
    : public CompositeTypeTemplate<CompositeType,
                                   const TupleType*,
                                   const Type*> {
  DECLARE_CONCRETE_IR_TYPE(FunctionType, CompositeType);

 public:
  ~FunctionType() override;

  const TupleType& parameters_type() const { return *member_at<0>(); }
  const Type& return_type() const { return *member_at<1>(); }

 private:
  friend class CompositeTypeFactory;

  FunctionType(const TupleType& parameters_type, const Type& return_type);

  DISALLOW_COPY_AND_ASSIGN(FunctionType);
};

//
// ReferenceType
//
class AOBA_IR_EXPORT ReferenceType
    : public CompositeTypeTemplate<CompositeType, const Type*> {
  DECLARE_CONCRETE_IR_TYPE(ReferenceType, CompositeType);

 public:
  ~ReferenceType() override;

  const Type& to() const { return *member_at<0>(); }

 private:
  friend class CompositeTypeFactory;

  explicit ReferenceType(const Type& return_type);

  DISALLOW_COPY_AND_ASSIGN(ReferenceType);
};

//
// TupleType
//
class AOBA_IR_EXPORT TupleType : public CompositeType {
  DECLARE_CONCRETE_IR_TYPE(TupleType, CompositeType);

 public:
  ~TupleType() override;

  const Type& get(size_t index) const { return *members_[index]; }
  auto members() const { return ReferenceRangeOf(members_); }
  size_t size() const { return members_.size(); }

 private:
  friend class CompositeTypeFactory;

  TupleType(Zone* zone, const std::vector<const Type*>& members);

  ZoneVector<const Type*> members_;

  DISALLOW_COPY_AND_ASSIGN(TupleType);
};

//
// UnionType
//
class AOBA_IR_EXPORT UnionType : public CompositeType {
  DECLARE_CONCRETE_IR_TYPE(UnionType, CompositeType);

 public:
  ~UnionType() override;

  auto members() const { return ReferenceRangeOf(members_); }

 private:
  friend class CompositeTypeFactory;

  UnionType(Zone* zone, const std::vector<const Type*>& members);

  ZoneVector<const Type*> members_;

  DISALLOW_COPY_AND_ASSIGN(UnionType);
};

}  // namespace ir
}  // namespace aoba

#endif  // AOBA_IR_COMPOSITE_TYPES_H_
