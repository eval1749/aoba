// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_ERROR_CODES_H_
#define AOBA_ANALYZER_ERROR_CODES_H_

#include "aoba/analyzer/public/error_codes.h"

namespace aoba {
namespace analyzer {

enum class ErrorCode {
  Start = kAnalyzerErrorCodeBase,
#define V(pass, reason) pass##_##reason,
  FOR_EACH_ANALYZER_ERROR_CODE(V)
#undef V
      End,
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_ERROR_CODES_H_
