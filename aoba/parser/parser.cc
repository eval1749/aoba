// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iterator>
#include <utility>
#include <vector>

#include "aoba/parser/parser.h"

#include "aoba/ast/compilation_units.h"
#include "aoba/ast/jsdoc_syntaxes.h"
#include "aoba/ast/node_factory.h"
#include "aoba/ast/node_traversal.h"
#include "aoba/ast/statements.h"
#include "aoba/ast/syntax.h"
#include "aoba/ast/tokens.h"
#include "aoba/base/error_sink.h"
#include "aoba/base/source_code.h"
#include "aoba/parser/lexer/lexer.h"
#include "aoba/parser/parser_error_codes.h"
#include "aoba/parser/public/parser_context.h"
#include "aoba/parser/utils/bracket_tracker.h"

namespace aoba {
namespace parser {

namespace {

bool IsCloseBracket(const ast::Node& token) {
  return token == ast::TokenKind::RightBrace ||
         token == ast::TokenKind::RightBracket ||
         token == ast::TokenKind::RightParenthesis;
}

// Returns true if |document| contains |@fileoviewview| tag.
bool HasJsDocTag(ast::TokenKind tag_id, const ast::Node& document) {
  for (const auto& element : ast::NodeTraversal::ChildNodesOf(document)) {
    if (element == ast::SyntaxCode::JsDocTag && element.child_at(0) == tag_id)
      return true;
  }
  return false;
}

std::unique_ptr<BracketTracker> NewBracketTracker(
    ErrorSink* error_sink,
    const SourceCodeRange& source_code_range) {
  const auto descriptions = std::vector<BracketTracker::Description>{
      {ast::TokenKind::LeftParenthesis,
       static_cast<int>(Parser::ErrorCode::ERROR_BRACKET_EXPECT_RPAREN),
       ast::TokenKind::RightParenthesis,
       static_cast<int>(Parser::ErrorCode::ERROR_BRACKET_UNEXPECT_RPAREN)},
      {ast::TokenKind::LeftBrace,
       static_cast<int>(Parser::ErrorCode::ERROR_BRACKET_EXPECT_RBRACE),
       ast::TokenKind::RightBrace,
       static_cast<int>(Parser::ErrorCode::ERROR_BRACKET_UNEXPECT_RBRACE)},
      {ast::TokenKind::LeftBracket,
       static_cast<int>(Parser::ErrorCode::ERROR_BRACKET_EXPECT_RBRACKET),
       ast::TokenKind::RightBracket,
       static_cast<int>(Parser::ErrorCode::ERROR_BRACKET_UNEXPECT_RBRACKET)},
  };

  return std::make_unique<BracketTracker>(error_sink, source_code_range,
                                          descriptions);
}

}  // namespace

//
// NodeRangeScope
//
Parser::NodeRangeScope::NodeRangeScope(Parser* parser)
    : offset_holder_(&parser->node_start_,
                     parser->CanPeekToken()
                         ? parser->PeekToken().range().start()
                         : parser->source_code().end().end()) {}

Parser::NodeRangeScope::~NodeRangeScope() = default;

//
// Parser
//
Parser::Parser(ParserContext* context,
               const SourceCodeRange& range,
               const ParserOptions& options)
    : bracket_tracker_(NewBracketTracker(&context->error_sink(), range)),
      context_(*context),
      lexer_(new Lexer(context, range, options)),
      options_(options) {}

Parser::~Parser() = default;

ast::NodeFactory& Parser::node_factory() const {
  return context_.node_factory();
}

const SourceCode& Parser::source_code() const {
  return lexer_->source_code();
}

void Parser::AddError(const ast::Node& token, ErrorCode error_code) {
  AddError(token.range(), error_code);
}

void Parser::AddError(const SourceCodeRange& range, ErrorCode error_code) {
  if (range.IsCollapsed())
    DCHECK_EQ(range, source_code().end());
  context_.error_sink().AddError(range, static_cast<int>(error_code));
}

void Parser::AddError(ErrorCode error_code) {
  if (CanPeekToken())
    return AddError(PeekToken(), error_code);
  AddError(source_code().end(), error_code);
}

void Parser::Advance() {
  if (!token_stack_.empty()) {
    token_stack_.pop();
    return;
  }
  is_separated_by_newline_ = false;
  if (!lexer_->CanPeekToken())
    return;
  lexer_->ConsumeToken();
  SkipCommentTokens();
}

bool Parser::CanPeekToken() const {
  if (!token_stack_.empty())
    return true;
  return lexer_->CanPeekToken();
}

const ast::Node& Parser::ConsumeToken() {
  auto& token = PeekToken();
  Advance();
  last_token_ = &token;
  return token;
}

bool Parser::ConsumeTokenIf(ast::TokenKind kind) {
  if (!CanPeekToken())
    return false;
  if (PeekToken() != kind)
    return false;
  ConsumeToken();
  return true;
}

bool Parser::ConsumeTokenIf(ast::SyntaxCode syntax) {
  if (!CanPeekToken())
    return false;
  if (PeekToken() != syntax)
    return false;
  ConsumeToken();
  return true;
}

void Parser::ExpectPunctuator(ast::TokenKind kind, ErrorCode error_code) {
  if (ConsumeTokenIf(kind))
    return;
  if (!CanPeekToken()) {
    // Unfinished statement or expression, e.g. just "foo" in source code.
    return AddError(
        source_code().Slice(node_start_, tokens_.back()->range().end()),
        error_code);
  }
  // We use previous token instead of current token, since current token may
  // be next line, e.g.
  // {
  //    foo()
  //    ~~~~~
  // }
  return AddError(
      source_code().Slice(node_start_,
                          (*std::next(tokens_.rbegin()))->range().end()),
      error_code);
}

void Parser::ExpectSemicolon() {
  if (!options_.disable_automatic_semicolon()) {
    if (!CanPeekToken())
      return;
    if (ConsumeTokenIf(ast::TokenKind::Semicolon))
      return;
    if (PeekToken() == ast::TokenKind::RightBrace)
      return;
    if (is_separated_by_newline_)
      return;
  }
  ExpectPunctuator(ast::TokenKind::Semicolon,
                   ErrorCode::ERROR_STATEMENT_EXPECT_SEMICOLON);
}

void Parser::Finish() {
  bracket_tracker_->Finish();
}

SourceCodeRange Parser::GetSourceCodeRange() const {
  return source_code().Slice(node_start_, last_token_->range().end());
}

const ast::Node& Parser::NewEmptyName() {
  return node_factory().NewEmpty(lexer_->location());
}

const ast::Node& Parser::PeekToken() const {
  if (token_stack_.empty())
    return lexer_->PeekToken();
  return *token_stack_.top();
}

void Parser::PushBackToken(const ast::Node& token) {
  token_stack_.push(&token);
}

const ast::Node& Parser::Run() {
  std::vector<const ast::Node*> statements;
  SkipCommentTokens();
  while (CanPeekToken()) {
    const auto& token = PeekToken();
    if (token != ast::SyntaxCode::JsDocDocument) {
      statements.push_back(&ParseStatement());
      continue;
    }

    if (HasJsDocTag(ast::TokenKind::AtFileOverview, token)) {
      const auto& document = ConsumeToken();
      if (file_overview_) {
        AddError(
            SourceCodeRange::Merge(file_overview_->range(), document.range()),
            ErrorCode::ERROR_JSDOC_MULTIPLE_FILE_OVERVIEWS);
      }
      file_overview_ = &document;
      statements.push_back(&document);
      continue;
    }

    statements.push_back(&ParseStatement());
    continue;
  }
  Finish();
  if (file_overview_ &&
      HasJsDocTag(ast::TokenKind::AtExterns, *file_overview_)) {
    return node_factory().NewExterns(source_code().range(), statements);
  }
  return node_factory().NewModule(source_code().range(), statements);
}

void Parser::SkipCommentTokens() {
  while (lexer_->CanPeekToken()) {
    if (lexer_->is_separated_by_newline())
      is_separated_by_newline_ = true;
    bracket_tracker_->Feed(PeekToken());
    tokens_.push_back(&PeekToken());
    if (PeekToken() != ast::SyntaxCode::Comment)
      return;
    lexer_->ConsumeToken();
  }
}

bool Parser::SkipToListElement() {
  const auto current_depth = bracket_tracker_->depth();
  DCHECK_GT(current_depth, 0u) << "We should call SkipListElement() in list.";
  while (CanPeekToken()) {
    if (current_depth != bracket_tracker_->depth()) {
      ConsumeToken();
      continue;
    }
    if (IsCloseBracket(PeekToken()))
      return false;
    if (ConsumeToken() == ast::TokenKind::Comma)
      return true;
  }
  return false;
}

}  // namespace parser
}  // namespace aoba
