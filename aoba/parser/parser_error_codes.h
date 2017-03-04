// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_PARSER_ERROR_CODES_H_
#define AOBA_PARSER_PARSER_ERROR_CODES_H_

#include "aoba/ast/error_codes.h"

#include "aoba/parser/parser.h"

namespace aoba {
namespace parser {

enum class Parser::ErrorCode {
  None = ast::kParserErrorCodeBase,
#define V(category, reason) ERROR_##category##_##reason,
  FOR_EACH_PARSER_ERROR_CODE(V)
#undef V
};

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_PARSER_PARSER_ERROR_CODES_H_
