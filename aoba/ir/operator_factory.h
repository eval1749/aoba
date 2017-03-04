// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_IR_OPERATOR_FACTORY_H_
#define AOBA_IR_OPERATOR_FACTORY_H_

#include <memory>
#include <string>

#include "base/macros.h"
#include "base/strings/string_piece.h"
#include "aoba/ir/ir_export.h"
#include "aoba/ir/operator_forward.h"
#include "aoba/ir/type_forward.h"

namespace aoba {

class Zone;

namespace ir {

//
// OperatorFactory
//
class AOBA_IR_EXPORT OperatorFactory final {
 public:
  explicit OperatorFactory(Zone* zone);
  ~OperatorFactory();

  const Operator& NewExit();
  const Operator& NewIf();
  const Operator& NewIfException();
  const Operator& NewIfFalse();
  const Operator& NewIfSuccess();
  const Operator& NewIfTrue();
  const Operator& NewLiteralBool(bool data);
  const Operator& NewLiteralFloat64(float64_t data);
  const Operator& NewLiteralInt64(int64_t data);
  const Operator& NewLiteralString(base::StringPiece16 data);
  const Operator& NewLiteralVoid();
  const Operator& NewProjection(size_t index);
  const Operator& NewRet();
  const Operator& NewTuple(size_t size);
  const Operator& NewStart();

 private:
  class Cache;

  Zone& zone_;

  // |cache_| takes |zone_| as constructor parameter.
  std::unique_ptr<Cache> cache_;

  DISALLOW_COPY_AND_ASSIGN(OperatorFactory);
};

}  // namespace ir
}  // namespace aoba

#endif  // AOBA_IR_OPERATOR_FACTORY_H_
