// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_UTILS_BRACKET_TRACKER_H_
#define AOBA_PARSER_UTILS_BRACKET_TRACKER_H_

#include <stack>
#include <utility>
#include <vector>

#include "base/macros.h"

namespace aoba {
class ErrorSink;
class SourceCodeRange;

namespace ast {
class Node;
enum class TokenKind;
}

namespace parser {

//
// BracketTracker
//
class BracketTracker final {
 public:
  struct Description {
    // The code of open bracket.
    ast::TokenKind open;

    // The error code for not closed open bracket.
    int open_error;

    // The code of close bracket.
    ast::TokenKind close;

    // The error code for mismatched close bracket.
    int close_error;
  };

  BracketTracker(ErrorSink* error_sink,
                 const SourceCodeRange& range,
                 const std::vector<Description>& descriptions);
  ~BracketTracker();

  ast::TokenKind close_bracket() const;
  size_t depth() const { return stack_.size(); }

  void Feed(const ast::Node& token);
  void Finish();

 private:
  void Check(const ast::Node& token, const Description& description);
  void Mark(const ast::Node& token, const Description& description);

  const std::vector<Description> descriptions_;
  ErrorSink& error_sink_;
  const SourceCodeRange& source_code_range_;
  const std::pair<ast::TokenKind, ast::TokenKind> min_max_;
  std::stack<std::pair<const ast::Node*, const Description*>> stack_;

  DISALLOW_COPY_AND_ASSIGN(BracketTracker);
};

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_PARSER_UTILS_BRACKET_TRACKER_H_
