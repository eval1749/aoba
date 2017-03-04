// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_TYPE_TRANSFORMER_H_
#define AOBA_ANALYZER_TYPE_TRANSFORMER_H_

#include "aoba/analyzer/context_user.h"

namespace aoba {
namespace analyzer {

//
// TypeTransformer
//
class TypeTransformer final : public ContextUser {
 public:
  explicit TypeTransformer(Context* context);
  ~TypeTransformer();

  // Transform AST type node to Type object.
  const Type& Transform(const ast::Node& node);
  const Type& TransformTypeApplication(const ast::Node& node);

 private:
  const Type& NewNullableType(const Type& type);

  const Type& TransformFunctionType(const ast::Node& node);
  const Type& TransformNonNullableType(const ast::Node& node);
  const Type& TransformRecordType(const ast::Node& node);
  const Type& TransformTypeName(const ast::Node& node);

  DISALLOW_COPY_AND_ASSIGN(TypeTransformer);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_TYPE_TRANSFORMER_H_
