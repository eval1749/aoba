// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_PUBLIC_PARSE_H_
#define AOBA_PARSER_PUBLIC_PARSE_H_

#include "base/macros.h"
#include "aoba/parser/public/parser_context.h"
#include "aoba/parser/public/parser_export.h"
#include "aoba/parser/public/parser_options.h"

namespace aoba {

namespace ast {
class Node;
}

class SourceCodeRange;

//
// The parser entry point.
//
AOBA_PARSER_EXPORT const ast::Node& Parse(ParserContext* context,
                                           const SourceCodeRange& range,
                                           const ParserOptions& options);

}  // namespace aoba

#endif  // AOBA_PARSER_PUBLIC_PARSE_H_
