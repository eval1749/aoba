// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_TYPE_TYPE_LEXER_H_
#define AOBA_PARSER_TYPE_TYPE_LEXER_H_

#include <memory>

#include "base/macros.h"
#include "base/strings/string16.h"
#include "aoba/parser/public/parser_options.h"

namespace aoba {
class ParserContext;
class SourceCode;
class SourceCodeRange;

namespace ast {
class Node;
class NodeFactory;
enum class TokenKind;
}
namespace parser {

class CharacterReader;
enum class TypeErrorCode;

enum class TypeLexerMode {
  Normal,
  JsDoc,
};

//
// TypeLexer
//
class TypeLexer final {
 public:
  TypeLexer(ParserContext* context,
            const SourceCodeRange& range,
            const ParserOptions& options,
            TypeLexerMode mode);
  ~TypeLexer();

  SourceCodeRange location() const;
  const SourceCodeRange& range() const;
  const SourceCode& source_code() const;

  bool CanPeekToken() const { return current_token_ != nullptr; }
  const ast::Node& ConsumeToken();
  const ast::Node& PeekToken() const;

 private:
  ast::NodeFactory& node_factory();

  void AddError(TypeErrorCode error_code);
  bool CanPeekChar() const;
  base::char16 ConsumeChar();
  bool ConsumeCharIf(base::char16 char_code);
  base::char16 PeekChar() const;

  const ast::Node* NextToken();

  // Factory members
  SourceCodeRange ComputeTokenRange() const;
  const ast::Node& NewPunctuator(ast::TokenKind kind);

  ParserContext& context_;
  const ast::Node* current_token_ = nullptr;
  const TypeLexerMode mode_;
  ParserOptions options_;
  const std::unique_ptr<CharacterReader> reader_;
  int token_start_ = 0;

  DISALLOW_COPY_AND_ASSIGN(TypeLexer);
};

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_PARSER_TYPE_TYPE_LEXER_H_
