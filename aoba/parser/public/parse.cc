// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/parser/public/parse.h"

#include "aoba/ast/node_factory.h"
#include "aoba/parser/parser.h"

namespace aoba {

//
// Parse; the entry point
//
const ast::Node& Parse(ParserContext* context,
                       const SourceCodeRange& range,
                       const ParserOptions& options) {
  parser::Parser parser(context, range, options);
  return parser.Run();
}

}  // namespace aoba
