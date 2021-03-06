// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <ostream>

#include "aoba/base/source_code_line.h"

#include "base/logging.h"
#include "aoba/base/source_code.h"

namespace aoba {

SourceCodeLine::SourceCodeLine(const SourceCodeRange& range, int number)
    : number_(number), range_(range) {
  DCHECK_GE(number_, 1);
#if DCHECK_IS_ON()
  for (auto offset = range.start(); offset < range.end() - 1; ++offset)
    DCHECK_NE(range.source_code().CharAt(offset), '\n') << offset;
#endif
}

SourceCodeLine::SourceCodeLine(const SourceCodeLine& other)
    : number_(other.number_), range_(other.range_) {}

SourceCodeLine::~SourceCodeLine() = default;

bool SourceCodeLine::operator==(const SourceCodeLine& other) const {
  if (range_ == other.range_) {
    DCHECK_EQ(number_, other.number_) << *this << ' ' << other;
    return true;
  }
  if (&range_.source_code() == &other.range_.source_code()) {
    DCHECK_NE(number_, other.number_) << *this << ' ' << other;
    return false;
  }
  return false;
}

bool SourceCodeLine::operator!=(const SourceCodeLine& other) const {
  return !operator==(other);
}

std::ostream& operator<<(std::ostream& ostream, const SourceCodeLine& line) {
  return ostream << line.range() << '(' << line.number() << ')';
}

}  // namespace aoba
