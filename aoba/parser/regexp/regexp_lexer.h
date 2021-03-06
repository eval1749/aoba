// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_REGEXP_REGEXP_LEXER_H_
#define AOBA_PARSER_REGEXP_REGEXP_LEXER_H_

#include <memory>

#include "base/macros.h"
#include "base/strings/string16.h"
#include "aoba/ast/node_factory.h"
#include "aoba/base/source_code_range.h"
#include "aoba/parser/public/parser_context.h"
#include "aoba/parser/public/parser_options.h"

namespace aoba {

namespace ast {
class Node;
class NodeFactory;
enum class TokenKind;
enum class RegExpAssertionKind;
enum class RegExpRepeatMethod;
}

class ParserContext;
class ParserOptions;
class SourceCode;

namespace parser {

class CharacterReader;
enum class RegExpErrorCode;

//
// RegExpLexer
//
class RegExpLexer final {
 public:
  RegExpLexer(ParserContext* context,
              const SourceCodeRange& range,
              const ParserOptions& options);
  ~RegExpLexer();

  int location() const;
  const SourceCode& source_code() const;

  bool CanPeekToken() const;
  const ast::Node& ConsumeToken();
  const ast::Node& PeekToken() const;

 private:
  ast::NodeFactory& node_factory() const;

  void AddError(RegExpErrorCode error_code);
  const ast::Node& HandleCharSet();
  int HandleDigits(int base);
  const ast::Node& HandleRepeat();
  bool IsSyntaxChar(base::char16 char_code) const;
  SourceCodeRange MakeTokenRange() const;
  const ast::Node* NextToken();
  const ast::Node& NewAssertion(ast::RegExpAssertionKind kind);
  const ast::Node& NewLiteral();
  const ast::Node& NewRepeat(ast::RegExpRepeatMethod method, int min, int max);
  const ast::Node& NewSyntaxChar(ast::TokenKind op);

  // CharacterReader helper function
  bool CanPeekChar() const;
  base::char16 ConsumeChar();
  bool ConsumeCharIf(base::char16 char_code);
  base::char16 PeekChar() const;

  ParserContext& context_;
  const ast::Node* current_token_ = nullptr;
  int group_ = 0;
  const ParserOptions& options_;
  const SourceCodeRange range_;
  std::unique_ptr<CharacterReader> reader_;
  int token_start_ = 0;

  DISALLOW_COPY_AND_ASSIGN(RegExpLexer);
};

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_PARSER_REGEXP_REGEXP_LEXER_H_
