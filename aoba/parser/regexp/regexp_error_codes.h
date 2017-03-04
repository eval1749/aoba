// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_REGEXP_REGEXP_ERROR_CODES_H_
#define AOBA_PARSER_REGEXP_REGEXP_ERROR_CODES_H_

#include "aoba/ast/error_codes.h"

namespace aoba {
namespace parser {

enum class RegExpErrorCode {
  None = ast::kRegExpErrorCodeBase,
#define V(category, reason) category##_##reason,
  FOR_EACH_REGEXP_ERROR_CODE(V)
#undef V
};

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_PARSER_REGEXP_REGEXP_ERROR_CODES_H_
