// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stack>
#include <utility>
#include <vector>

#include "aoba/parser/jsdoc/jsdoc_parser.h"

#include "base/auto_reset.h"
#include "aoba/ast/jsdoc_syntaxes.h"
#include "aoba/ast/jsdoc_tags.h"
#include "aoba/ast/node.h"
#include "aoba/ast/node_factory.h"
#include "aoba/ast/syntax_forward.h"
#include "aoba/ast/tokens.h"
#include "aoba/base/error_sink.h"
#include "aoba/base/source_code.h"
#include "aoba/base/source_code_range.h"
#include "aoba/parser/jsdoc/jsdoc_error_codes.h"
#include "aoba/parser/public/parser_context.h"
#include "aoba/parser/type/type_lexer.h"
#include "aoba/parser/type/type_parser.h"
#include "aoba/parser/utils/character_reader.h"
#include "aoba/parser/utils/lexer_utils.h"

namespace aoba {
namespace parser {

namespace {

enum class TagSyntax {
  Unknown,
#define V(name, ...) name,
  FOR_EACH_JSDOC_TAG_SYNTAX(V)
#undef V
};

TagSyntax TagSyntaxOf(const ast::Node& name) {
#define V(underscore, capital, syntax)     \
  if (name == ast::TokenKind::At##capital) \
    return TagSyntax::syntax;
  FOR_EACH_JSDOC_TAG_NAME(V)
#undef V
  return TagSyntax::Unknown;
}

}  // namespace

//
// JsDocParser::NodeRangeScope
//
class JsDocParser::NodeRangeScope final {
 public:
  explicit NodeRangeScope(JsDocParser* parser)
      : auto_reset_(&parser->node_start_, parser->reader_->location()) {}

  ~NodeRangeScope() = default;

 private:
  base::AutoReset<int> auto_reset_;

  DISALLOW_COPY_AND_ASSIGN(NodeRangeScope);
};

//
// JsDocParser
//
JsDocParser::JsDocParser(ParserContext* context,
                         const SourceCodeRange& range,
                         const ParserOptions& options)
    : context_(*context),
      node_start_(range.start()),
      options_(options),
      reader_(new CharacterReader(range)) {}

JsDocParser::~JsDocParser() = default;

// ErrorSink utility functions
void JsDocParser::AddError(const SourceCodeRange& range,
                           JsDocErrorCode error_code) {
  context_.error_sink().AddError(range, error_code);
}

void JsDocParser::AddError(int start, int end, JsDocErrorCode error_code) {
  AddError(source_code().Slice(start, end), error_code);
}

void JsDocParser::AddError(JsDocErrorCode error_code) {
  AddError(ComputeNodeRange(), error_code);
}

// CharacterReader utility functions
int JsDocParser::location() const {
  return reader_->location();
}

const SourceCode& JsDocParser::source_code() const {
  return reader_->source_code();
}

bool JsDocParser::CanPeekChar() const {
  return reader_->CanPeekChar();
}

base::char16 JsDocParser::ConsumeChar() {
  return reader_->ConsumeChar();
}

bool JsDocParser::ConsumeCharIf(base::char16 char_code) {
  return reader_->ConsumeCharIf(char_code);
}

base::char16 JsDocParser::PeekChar() const {
  return reader_->PeekChar();
}

// Factory utility functions
ast::NodeFactory& JsDocParser::node_factory() {
  return context_.node_factory();
}

SourceCodeRange JsDocParser::ComputeNodeRange() const {
  return source_code().Slice(node_start_, location());
}

const ast::Node& JsDocParser::NewDocument(
    const std::vector<const ast::Node*>& nodes) {
  return node_factory().NewJsDocDocument(ComputeNodeRange(), nodes);
}

const ast::Node& JsDocParser::NewName() {
  return node_factory().NewName(ComputeNodeRange());
}

const ast::Node& JsDocParser::NewTagName() {
  return node_factory().NewName(ComputeNodeRange());
}

const ast::Node& JsDocParser::NewTagWithVector(
    const ast::Node& name,
    const std::vector<const ast::Node*>& parameters) {
  return node_factory().NewJsDocTag(ComputeNodeRange(), name, parameters);
}

const ast::Node& JsDocParser::NewText(const SourceCodeRange& range) {
  return node_factory().NewJsDocText(range);
}

const ast::Node& JsDocParser::NewText(int start, int end) {
  return NewText(source_code().Slice(start, end));
}

const ast::Node& JsDocParser::NewText() {
  return NewText(ComputeNodeRange());
}

// Parsing functions
// The entry point of JsDoc parser.
const ast::Node* JsDocParser::Parse() {
  NodeRangeScope scope(this);
  SkipWhitespaces();
  std::vector<const ast::Node*> nodes;
  auto number_of_tags = 0;
  auto text_start = location();
  while (CanPeekChar()) {
    const auto text_end = SkipToBlockTag();
    if (text_end > text_start)
      nodes.push_back(&NewText(text_start, text_end));
    if (!CanPeekChar())
      break;
    ++number_of_tags;
    NodeRangeScope scope(this);
    nodes.push_back(&ParseTag(ParseTagName()));
    SkipWhitespaces();
    text_start = location();
  }
  if (number_of_tags == 0)
    return nullptr;
  return &NewDocument(nodes);
}

// Extract text without leading and trailing whitespace.
const ast::Node& JsDocParser::ParseDescription() {
  SkipWhitespaces();
  const auto text_start = reader_->location();
  if (CanPeekChar() && IsLineTerminator(PeekChar())) {
    ConsumeChar();
    return NewText(text_start, text_start);
  }
  auto text_end = SkipToBlockTag();
  return NewText(text_start, text_end);
}

const ast::Node& JsDocParser::ParseModifies() {
  SkipWhitespaces();
  if (!ConsumeCharIf(kLeftBrace))
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_LBRACE);
  const auto& name = ParseName();
  if (name != ast::TokenKind::Arguments && name != ast::TokenKind::This)
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_ARGUMENTS_OR_THIS);
  SkipWhitespaces();
  if (!ConsumeCharIf(kRightBrace))
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_RBRACE);
  return name;
}

// Returns list of comma separated words.
std::vector<const ast::Node*> JsDocParser::ParseNameList() {
  SkipWhitespaces();
  NodeRangeScope scope(this);
  if (!ConsumeCharIf(kLeftBrace)) {
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_LBRACE);
    return {&NewText()};
  }
  const auto& names = ParseNames();
  if (!ConsumeCharIf(kRightBrace))
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_RBRACE);
  return std::move(names);
}

const ast::Node& JsDocParser::ParseName() {
  SkipWhitespaces();
  NodeRangeScope scope(this);
  while (CanPeekChar() && IsIdentifierPart(PeekChar()))
    ConsumeChar();
  if (node_start_ == location()) {
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_NAME);
    return node_factory().NewEmpty(ComputeNodeRange());
  }
  return NewName();
}

std::vector<const ast::Node*> JsDocParser::ParseNames() {
  std::vector<const ast::Node*> names;
  SkipWhitespaces();
  while (CanPeekChar()) {
    if (!IsIdentifierStart(PeekChar())) {
      AddError(JsDocErrorCode::ERROR_TAG_EXPECT_NAME);
      break;
    }
    names.push_back(&ParseName());
    SkipWhitespaces();
    if (!ConsumeCharIf(','))
      break;
    SkipWhitespaces();
  }
  if (names.empty())
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_NAMES);
  return names;
}

// Extract text until newline
const ast::Node& JsDocParser::ParseSingleLine() {
  SkipWhitespaces();
  const auto text_start = location();
  auto text_end = text_start;
  while (CanPeekChar()) {
    const auto char_code = ConsumeChar();
    if (IsLineTerminator(char_code))
      break;
    if (IsWhitespace(char_code))
      continue;
    text_end = location();
  }
  return NewText(text_start, text_end);
}

// Called after consuming '@'.
const ast::Node& JsDocParser::ParseTag(const ast::Node& tag_name) {
  switch (TagSyntaxOf(tag_name)) {
    case TagSyntax::Description:
      SkipWhitespaces();
      return NewTag(tag_name, ParseDescription());
    case TagSyntax::Modifies:
      return NewTag(tag_name, ParseModifies());
    case TagSyntax::NameList:
      return NewTagWithVector(tag_name, ParseNameList());
    case TagSyntax::None:
      return NewTag(tag_name);
    case TagSyntax::OptionalType:
      SkipWhitespaces();
      if (CanPeekChar() && PeekChar() == kLeftBrace)
        return NewTag(tag_name, ParseType());
      return NewTag(tag_name);
    case TagSyntax::SingleLine:
      return NewTag(tag_name, ParseSingleLine());
    case TagSyntax::Type:
      return NewTag(tag_name, ParseType());
    case TagSyntax::TypeDescription: {
      auto& type = ParseType();
      auto& description = ParseDescription();
      return NewTag(tag_name, type, description);
    }
    case TagSyntax::TypeNames: {
      const auto& names = ParseNames();
      std::vector<const ast::Node*> type_names(names.size());
      type_names.resize(0);
      for (const auto* name : names)
        type_names.push_back(&node_factory().NewTypeName(*name));
      return NewTagWithVector(tag_name, type_names);
    }
    case TagSyntax::TypeParameterDescription: {
      auto& type = ParseType();
      const auto& maybe_name = ParseName();
      auto& name_reference =
          maybe_name.Is<ast::Name>()
              ? node_factory().NewReferenceExpression(maybe_name)
              : maybe_name;
      auto& description = ParseDescription();
      return NewTag(tag_name, type, name_reference, description);
    }
    case TagSyntax::Unknown:
      AddError(tag_name.range(), JsDocErrorCode::ERROR_TAG_UNKNOWN_TAG);
      return NewTag(tag_name);
  }
  NOTREACHED() << "We should handle " << tag_name;
  return NewTag(tag_name);
}

const ast::Node& JsDocParser::ParseTagName() {
  DCHECK_EQ(PeekChar(), '@');
  NodeRangeScope scope(this);
  ConsumeChar();
  while (CanPeekChar() && IsIdentifierPart(PeekChar()))
    ConsumeChar();
  return NewTagName();
}

const ast::Node& JsDocParser::ParseType() {
  SkipWhitespaces();
  NodeRangeScope scope(this);
  if (!ConsumeCharIf(kLeftBrace)) {
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_LBRACE);
    return NewText();
  }
  const auto type_start = reader_->location();
  auto number_of_braces = 0;
  while (CanPeekChar()) {
    if (ConsumeCharIf(kLeftBrace)) {
      ++number_of_braces;
      continue;
    }
    if (PeekChar() == kRightBrace) {
      if (number_of_braces == 0)
        break;
      --number_of_braces;
      ConsumeChar();
      continue;
    }
    ConsumeChar();
  }
  TypeParser parser(&context_,
                    source_code().Slice(type_start, reader_->location()),
                    options_, TypeLexerMode::JsDoc);
  auto& type = parser.Parse();
  if (!ConsumeCharIf(kRightBrace))
    AddError(JsDocErrorCode::ERROR_TAG_EXPECT_RBRACE);
  return type;
}

int JsDocParser::SkipToBlockTag() {
  SkipWhitespaces();
  enum class State {
    Brace,
    InlineTag,
    Normal,
    Whitespace,
  } state = State::Whitespace;
  auto text_end = reader_->location();
  auto inline_tag_start = 0;
  while (CanPeekChar()) {
    switch (state) {
      case State::Brace:
        if (ConsumeCharIf('@')) {
          state = State::InlineTag;
          continue;
        }
        if (ConsumeCharIf(kLeftBrace))
          continue;
        state = State::Normal;
        continue;
      case State::Normal:
        if (IsWhitespace(PeekChar())) {
          ConsumeChar();
          state = State::Whitespace;
          continue;
        }
        text_end = reader_->location() + 1;
        if (ConsumeChar() == kLeftBrace) {
          inline_tag_start = reader_->location() - 1;
          state = State::Brace;
          continue;
        }
        continue;
      case State::InlineTag:
        if (ConsumeChar() != kRightBrace)
          continue;
        state = State::Normal;
        text_end = reader_->location();
        continue;
      case State::Whitespace:
        if (IsWhitespace(PeekChar())) {
          ConsumeChar();
          continue;
        }
        if (PeekChar() == '@')
          return text_end;
        state = State::Normal;
        continue;
    }
    NOTREACHED() << "We have missing case for state="
                 << static_cast<int>(state);
  }
  if (state == State::InlineTag) {
    AddError(inline_tag_start, reader_->location(),
             JsDocErrorCode::ERROR_TAG_EXPECT_RBRACE);
  }
  return text_end;
}

void JsDocParser::SkipWhitespaces() {
  while (CanPeekChar() && IsWhitespace(PeekChar()) &&
         !IsLineTerminator(PeekChar())) {
    ConsumeChar();
  }
}

}  // namespace parser
}  // namespace aoba
