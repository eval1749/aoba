// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_SOURCE_CODE_LINE_CACHE_H_
#define AOBA_BASE_SOURCE_CODE_LINE_CACHE_H_

#include <vector>

#include "base/macros.h"
#include "aoba/base/source_code_line.h"

namespace aoba {

class AOBA_BASE_EXPORT SourceCodeLine::Cache final {
 public:
  Cache(const SourceCode& source_code);
  ~Cache();

  const SourceCode& source_code() const { return source_code_; }

  SourceCodeLine Get(int offset) const;

 private:
  mutable std::vector<int> offsets_;
  mutable int runner_ = 0;
  const SourceCode& source_code_;

  DISALLOW_COPY_AND_ASSIGN(Cache);
};

}  // namespace aoba

#endif  // AOBA_BASE_SOURCE_CODE_LINE_CACHE_H_
