// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_UTILS_CHARACTER_READER_H_
#define AOBA_PARSER_UTILS_CHARACTER_READER_H_

#include "base/macros.h"
#include "base/strings/string16.h"

namespace aoba {

class SourceCode;
class SourceCodeRange;

namespace parser {

class CharacterReader final {
 public:
  explicit CharacterReader(const SourceCodeRange& range);
  ~CharacterReader();

  int location() const;
  const SourceCodeRange& range() const { return range_; }
  const SourceCode& source_code() const;

  bool CanPeekChar() const;
  base::char16 ConsumeChar();

  // Returns true if |PeekChar()| is |expected_char| and advance to next
  // character.
  bool ConsumeCharIf(base::char16 expected_char);

  // For handling an RegExp starts with "=".
  void MoveBackward();

  // Should be called after |CanPeekChar()|.
  void MoveForward();

  base::char16 PeekChar() const;

 private:
  void FetchChar();

  int current_char_ = -1;
  int current_char_offset_;
  const SourceCodeRange& range_;

  DISALLOW_COPY_AND_ASSIGN(CharacterReader);
};

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_PARSER_UTILS_CHARACTER_READER_H_
