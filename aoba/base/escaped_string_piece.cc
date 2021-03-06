// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits>
#include <ostream>

#include "aoba/base/escaped_string_piece.h"

namespace aoba {

EscapedStringPiece16::EscapedStringPiece16(base::StringPiece16 piece,
                                           char delimiter,
                                           size_t max_length)
    : delimiter_(delimiter), max_length_(max_length), string_(piece) {}

EscapedStringPiece16::EscapedStringPiece16(base::StringPiece16 piece,
                                           char delimiter)
    : EscapedStringPiece16(piece,
                           delimiter,
                           std::numeric_limits<size_t>::max()) {}

EscapedStringPiece16::~EscapedStringPiece16() = default;

std::ostream& operator<<(std::ostream& ostream,
                         const EscapedStringPiece16& escaped) {
  static const char* const kHexDigits = "0123456789ABCDEF";
  static const char* const kEscapes = "01234567btnvfr";
  const auto delimiter = escaped.delimiter();
  ostream << delimiter;
  size_t counter = 0;
  for (const auto char_code : escaped.string()) {
    if (counter == escaped.max_length()) {
      ostream << "...";
      break;
    }
    ++counter;
    if (char_code == delimiter) {
      ostream << '\\' << delimiter;
      continue;
    }
    if (char_code == '\\') {
      ostream << "\\\\";
      continue;
    }
    if (char_code >= 8 && char_code <= 0x0D) {
      ostream << '\\' << kEscapes[char_code];
      continue;
    }
    if (char_code >= ' ' && char_code <= 0x7E) {
      ostream << static_cast<char>(char_code);
      continue;
    }
    ostream << "\\u" << kHexDigits[(char_code >> 12) & 15]
            << kHexDigits[(char_code >> 8) & 15]
            << kHexDigits[(char_code >> 4) & 15] << kHexDigits[char_code & 15];
  }
  return ostream << delimiter;
}

}  // namespace aoba
