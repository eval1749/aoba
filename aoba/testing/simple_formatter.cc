// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ostream>
#include <string>

#include "aoba/testing/simple_formatter.h"

#include "base/strings/utf_string_conversions.h"
#include "aoba/ast/bindings.h"
#include "aoba/ast/compilation_units.h"
#include "aoba/ast/declarations.h"
#include "aoba/ast/error_codes.h"
#include "aoba/ast/expressions.h"
#include "aoba/ast/jsdoc_syntaxes.h"
#include "aoba/ast/literals.h"
#include "aoba/ast/node_traversal.h"
#include "aoba/ast/regexp.h"
#include "aoba/ast/statements.h"
#include "aoba/ast/tokens.h"
#include "aoba/ast/types.h"
#include "aoba/base/source_code.h"

namespace aoba {
namespace parser {

namespace {

//
// Context
//
struct Context {
  int depth = 0;

  Context() = default;
};

struct Printable {
  Context* context;
  const ast::Node* node;
};

std::ostream& operator<<(std::ostream& ostream, const Printable& printable) {
  const auto& node = printable.node;
  return ostream << node;
}

}  // namespace

Formatted AsFormatted(const ast::Node& node) {
  return Formatted{&node};
}

std::ostream& operator<<(std::ostream& ostream, const Formatted& formatted) {
  Context context;
  return ostream << Printable{&context, formatted.node};
}

}  // namespace parser
}  // namespace aoba
