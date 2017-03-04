// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_LEXER_LEXER_ERROR_CODES_H_
#define AOBA_PARSER_LEXER_LEXER_ERROR_CODES_H_

#include "aoba/ast/error_codes.h"

#include "aoba/parser/lexer/lexer.h"

namespace aoba {
namespace parser {

enum class Lexer::ErrorCode {
  None = ast::kLexerErrorCodeBase,
#define V(category, reason) category##_##reason,
  FOR_EACH_LEXER_ERROR_CODE(V)
#undef V
};

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_PARSER_LEXER_LEXER_ERROR_CODES_H_
