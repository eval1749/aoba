// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_SOURCE_CODE_LINE_H_
#define AOBA_BASE_SOURCE_CODE_LINE_H_

#include <iosfwd>

#include "aoba/base/source_code_range.h"

namespace aoba {

// Line number starts with one.
class AOBA_BASE_EXPORT SourceCodeLine final {
 public:
  class Cache;

  SourceCodeLine(const SourceCodeLine& other);
  ~SourceCodeLine();

  int end() const { return range_.end(); }
  int number() const { return number_; }
  const SourceCodeRange& range() const { return range_; }
  int size() const { return range_.size(); }
  int start() const { return range_.start(); }

  bool operator==(const SourceCodeLine& other) const;
  bool operator!=(const SourceCodeLine& other) const;

 private:
  friend class SourceCodeLineTest;

  SourceCodeLine(const SourceCodeRange& range, int number);

  int number_ = 0;
  SourceCodeRange range_;
};

AOBA_BASE_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                             const SourceCodeLine& line);
}  // namespace aoba

#endif  // AOBA_BASE_SOURCE_CODE_LINE_H_
