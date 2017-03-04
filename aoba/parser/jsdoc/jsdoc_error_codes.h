// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_JSDOC_JSDOC_ERROR_CODES_H_
#define AOBA_PARSER_JSDOC_JSDOC_ERROR_CODES_H_

#include "aoba/ast/error_codes.h"

namespace aoba {
namespace parser {

enum class JsDocErrorCode {
  None = ast::kJsDocErrorCodeBase,
#define V(category, reason) ERROR_##category##_##reason,
  FOR_EACH_JSDOC_ERROR_CODE(V)
#undef V
};

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_PARSER_JSDOC_JSDOC_ERROR_CODES_H_
