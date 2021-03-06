// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/parser/type/type_lexer.h"

#include "aoba/ast/node_factory.h"
#include "aoba/ast/tokens.h"
#include "aoba/base/error_sink.h"
#include "aoba/base/source_code.h"
#include "aoba/base/source_code_range.h"
#include "aoba/parser/public/parser_context.h"
#include "aoba/parser/type/type_error_codes.h"
#include "aoba/parser/utils/character_reader.h"
#include "aoba/parser/utils/lexer_utils.h"

namespace aoba {
namespace parser {

//
// TypeLexer
//
TypeLexer::TypeLexer(ParserContext* context,
                     const SourceCodeRange& range,
                     const ParserOptions& options,
                     TypeLexerMode mode)
    : context_(*context),
      mode_(mode),
      options_(options),
      reader_(new CharacterReader(range)) {
  current_token_ = NextToken();
}

TypeLexer::~TypeLexer() = default;

SourceCodeRange TypeLexer::location() const {
  return source_code().Slice(reader_->location(), reader_->location());
}

ast::NodeFactory& TypeLexer::node_factory() {
  return context_.node_factory();
}

const SourceCodeRange& TypeLexer::range() const {
  return reader_->range();
}

const SourceCode& TypeLexer::source_code() const {
  return reader_->source_code();
}

void TypeLexer::AddError(TypeErrorCode error_code) {
  context_.error_sink().AddError(ComputeTokenRange(),
                                 static_cast<int>(error_code));
}

bool TypeLexer::CanPeekChar() const {
  return reader_->CanPeekChar();
}

SourceCodeRange TypeLexer::ComputeTokenRange() const {
  return source_code().Slice(token_start_, reader_->location());
}

base::char16 TypeLexer::ConsumeChar() {
  return reader_->ConsumeChar();
}

bool TypeLexer::ConsumeCharIf(base::char16 char_code) {
  if (!CanPeekChar() || PeekChar() != char_code)
    return false;
  ConsumeChar();
  return true;
}

const ast::Node& TypeLexer::ConsumeToken() {
  auto& token = PeekToken();
  current_token_ = NextToken();
  return token;
}

base::char16 TypeLexer::PeekChar() const {
  return reader_->PeekChar();
}

const ast::Node& TypeLexer::PeekToken() const {
  DCHECK(current_token_);
  return *current_token_;
}

const ast::Node& TypeLexer::NewPunctuator(ast::TokenKind kind) {
  return node_factory().NewPunctuator(ComputeTokenRange(), kind);
}

const ast::Node* TypeLexer::NextToken() {
start_over:
  auto found_newline = false;
  while (CanPeekChar() && IsWhitespace(PeekChar())) {
    if (IsLineTerminator(PeekChar()))
      found_newline = true;
    ConsumeChar();
  }
  if (!CanPeekChar())
    return nullptr;
  if (mode_ == TypeLexerMode::JsDoc && found_newline && ConsumeCharIf('*'))
    goto start_over;
  token_start_ = reader_->location();
  switch (PeekChar()) {
    case '!':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::LogicalNot);
    case '(':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::LeftParenthesis);
    case ')':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::RightParenthesis);
    case ',':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::Comma);
    case '*':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::Times);
    case ':':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::Colon);
    case '<':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::LessThan);
    case '=':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::Equal);
    case '>':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::GreaterThan);
    case '?':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::Question);
    case '[':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::LeftBracket);
    case ']':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::RightBracket);
    case '{':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::LeftBrace);
    case '|':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::BitOr);
    case '}':
      ConsumeChar();
      return &NewPunctuator(ast::TokenKind::RightBrace);
    case '.': {
      auto number_of_dots = 0;
      while (ConsumeCharIf('.'))
        ++number_of_dots;
      if (number_of_dots == 1)
        return &NewPunctuator(ast::TokenKind::Dot);
      if (number_of_dots == 3)
        return &NewPunctuator(ast::TokenKind::DotDotDot);
      AddError(TypeErrorCode::ERROR_TYPE_UNEXPECT_DOT);
      return &NewPunctuator(ast::TokenKind::Invalid);
    }
  }
  if (IsIdentifierStart(PeekChar())) {
    ConsumeChar();
    while (CanPeekChar() && IsIdentifierPart(PeekChar()))
      ConsumeChar();
    return &node_factory().NewName(ComputeTokenRange());
  }
  ConsumeChar();
  return &NewPunctuator(ast::TokenKind::Invalid);
}

}  // namespace parser
}  // namespace aoba
