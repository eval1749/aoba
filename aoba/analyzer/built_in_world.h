// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_BUILT_IN_WORLD_H_
#define AOBA_ANALYZER_BUILT_IN_WORLD_H_

#include <memory>
#include <vector>

#include "base/macros.h"

namespace base {
template <typename T>
struct DefaultSingletonTraits;
}

namespace aoba {
namespace ast {
class Node;
enum class TokenKind;
}
namespace analyzer {

//
// BuiltInWorld
//
class BuiltInWorld final {
 public:
  ~BuiltInWorld();

  const ast::Node& global_module() const { return global_module_; }
  const std::vector<ast::TokenKind>& primitive_types() const;

  const ast::Node& NameOf(ast::TokenKind kind) const;
  const ast::Node& TypeOf(ast::TokenKind kind) const;

  static BuiltInWorld* GetInstance();

 private:
  friend struct base::DefaultSingletonTraits<BuiltInWorld>;
  class Private;

  BuiltInWorld();

  std::unique_ptr<Private> private_;
  const ast::Node& global_module_;

  DISALLOW_COPY_AND_ASSIGN(BuiltInWorld);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_BUILT_IN_WORLD_H_
