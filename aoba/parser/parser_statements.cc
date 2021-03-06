// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iterator>
#include <vector>

#include "aoba/parser/parser.h"

#include "aoba/ast/bindings.h"
#include "aoba/ast/declarations.h"
#include "aoba/ast/expressions.h"
#include "aoba/ast/node.h"
#include "aoba/ast/node_factory.h"
#include "aoba/ast/statements.h"
#include "aoba/ast/tokens.h"
#include "aoba/base/source_code.h"
#include "aoba/parser/lexer/lexer.h"
#include "aoba/parser/parser_error_codes.h"

namespace aoba {
namespace parser {

namespace {

const ast::Node& BindingIntializerOf(const ast::Node& binding) {
  if (binding.Is<ast::BindingNameElement>())
    return ast::BindingNameElement::InitializerOf(binding);
  if (binding.Is<ast::ArrayBindingPattern>())
    return ast::ArrayBindingPattern::InitializerOf(binding);
  if (binding.Is<ast::ObjectBindingPattern>())
    return ast::ObjectBindingPattern::InitializerOf(binding);
  NOTREACHED();
  return binding;
}

bool HasInitializer(const ast::Node& node) {
  if (node.Is<ast::BindingNameElement>()) {
    const auto& initializer = ast::BindingNameElement::InitializerOf(node);
    return !initializer.Is<ast::ElisionExpression>();
  }
  if (node.Is<ast::ArrayBindingPattern>()) {
    const auto& initializer = ast::ArrayBindingPattern::InitializerOf(node);
    return !initializer.Is<ast::ElisionExpression>();
  }
  if (node.Is<ast::ObjectBindingPattern>()) {
    const auto& initializer = ast::ObjectBindingPattern::InitializerOf(node);
    return !initializer.Is<ast::ElisionExpression>();
  }
  return false;
}

bool IsInExpression(const ast::Node& expression) {
  return expression.Is<ast::BinaryExpression>() &&
         ast::BinaryExpression::OperatorOf(expression) == ast::TokenKind::In;
}

bool IsLeftHandSide(const ast::Node& expression) {
  return expression.Is<ast::ReferenceExpression>() ||
         expression.Is<ast::ComputedMemberExpression>() ||
         expression.Is<ast::MemberExpression>() ||
         expression.Is<ast::ObjectInitializer>();
}

}  // namespace

//
// Functions for parsing statements
//

// Called after before consuming ':'
const ast::Node& Parser::HandleLabeledStatement(const ast::Node& label) {
  auto& colon = ConsumeToken();
  DCHECK_EQ(colon, ast::TokenKind::Colon);
  if (!options_.disable_automatic_semicolon()) {
    if (!CanPeekToken() || PeekToken() == ast::TokenKind::RightBrace) {
      auto& statement =
          NewEmptyStatement(SourceCodeRange::CollapseToEnd(colon.range()));
      return node_factory().NewLabeledStatement(GetSourceCodeRange(), label,
                                                statement);
    }
  }
  auto& statement = ParseStatement();
  return node_factory().NewLabeledStatement(GetSourceCodeRange(), label,
                                            statement);
}

const ast::Node& Parser::NewEmptyStatement(const SourceCodeRange& range) {
  DCHECK(range.IsCollapsed()) << range;
  return node_factory().NewEmptyStatement(range);
}

const ast::Node& Parser::NewInvalidStatement(ErrorCode error_code) {
  AddError(GetSourceCodeRange(), error_code);
  return node_factory().NewInvalidStatement(GetSourceCodeRange(),
                                            static_cast<int>(error_code));
}

const ast::Node& Parser::ParseJsDocAsStatement() {
  NodeRangeScope scope(this);
  const auto& jsdoc = ConsumeToken();
  const auto& statement = ParseStatement();
  if (statement == ast::SyntaxCode::Annotation) {
    // We don't allow statement/expression has more than one annotation.
    AddError(jsdoc, ErrorCode::ERROR_STATEMENT_UNEXPECT_ANNOTATION);
    return node_factory().NewAnnotation(GetSourceCodeRange(), jsdoc, statement);
  }
  if (statement != ast::SyntaxCode::ExpressionStatement)
    return node_factory().NewAnnotation(GetSourceCodeRange(), jsdoc, statement);
  const auto& expression = ast::ExpressionStatement::ExpressionOf(statement);
  if (expression.Is<ast::AssignmentExpression>() &&
      ast::AssignmentExpression::OperatorOf(expression) ==
          ast::TokenKind::Equal &&
      IsMemberExpression(
          ast::AssignmentExpression::LeftHandSideOf(expression))) {
    return node_factory().NewAnnotation(
        GetSourceCodeRange(), jsdoc,
        node_factory().NewDeclaration(
            expression.range(),
            ast::AssignmentExpression::LeftHandSideOf(expression),
            ast::AssignmentExpression::RightHandSideOf(expression)));
  }
  if (IsMemberExpression(expression)) {
    return node_factory().NewAnnotation(
        GetSourceCodeRange(), jsdoc,
        node_factory().NewDeclaration(expression.range(), expression,
                                      NewElisionExpression(expression)));
  }
  AddError(jsdoc, ErrorCode::ERROR_STATEMENT_UNEXPECT_ANNOTATION);
  return node_factory().NewAnnotation(GetSourceCodeRange(), jsdoc, statement);
}

const ast::Node& Parser::ParseBlockStatement() {
  NodeRangeScope scope(this);
  DCHECK_EQ(PeekToken(), ast::TokenKind::LeftBrace);
  ConsumeToken();
  std::vector<const ast::Node*> statements;
  while (CanPeekToken()) {
    if (ConsumeTokenIf(ast::TokenKind::RightBrace))
      break;
    if (ConsumeTokenIf(ast::SyntaxCode::Comment))
      continue;
    statements.push_back(&ParseStatement());
  }
  return node_factory().NewBlockStatement(GetSourceCodeRange(), statements);
}

const ast::Node& Parser::ParseBreakStatement() {
  ConsumeToken();
  if (is_separated_by_newline_) {
    if (options_.disable_automatic_semicolon())
      AddError(ErrorCode::ERROR_STATEMENT_UNEXPECT_NEWLINE);
    return node_factory().NewBreakStatement(GetSourceCodeRange(),
                                            NewEmptyName());
  }
  auto& label = CanPeekToken() && PeekToken() == ast::SyntaxCode::Name
                    ? ConsumeToken()
                    : NewEmptyName();
  ExpectSemicolon();
  return node_factory().NewBreakStatement(GetSourceCodeRange(), label);
}

const ast::Node& Parser::ParseCaseClause() {
  NodeRangeScope scope(this);
  DCHECK_EQ(PeekToken(), ast::TokenKind::Case);
  ConsumeToken();
  auto& expression = ParseExpression();
  if (!CanPeekToken() || PeekToken() != ast::TokenKind::Colon) {
    AddError(ErrorCode::ERROR_STATEMENT_EXPECT_COLON);
  } else {
    auto& colon = ConsumeToken();
    if (CanPeekToken() && PeekToken() == ast::TokenKind::RightBrace) {
      if (options_.disable_automatic_semicolon())
        AddError(ErrorCode::ERROR_STATEMENT_EXPECT_SEMICOLON);
      return node_factory().NewCaseClause(
          GetSourceCodeRange(), expression,
          NewEmptyStatement(SourceCodeRange::CollapseToEnd(colon.range())));
    }
  }
  auto& statement = ParseStatement();
  return node_factory().NewCaseClause(GetSourceCodeRange(), expression,
                                      statement);
}

const ast::Node& Parser::ParseConstStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::Const);
  ConsumeToken();
  const auto& elements = ParseBindingElements();
  ExpectSemicolon();
  return node_factory().NewConstStatement(GetSourceCodeRange(), elements);
}

const ast::Node& Parser::ParseContinueStatement() {
  ConsumeToken();
  if (is_separated_by_newline_) {
    if (options_.disable_automatic_semicolon())
      AddError(ErrorCode::ERROR_STATEMENT_UNEXPECT_NEWLINE);
    return node_factory().NewContinueStatement(GetSourceCodeRange(),
                                               NewEmptyName());
  }
  auto& label = CanPeekToken() && PeekToken() == ast::SyntaxCode::Name
                    ? ConsumeToken()
                    : NewEmptyName();
  ExpectSemicolon();
  return node_factory().NewContinueStatement(GetSourceCodeRange(), label);
}

const ast::Node& Parser::ParseDefaultLabel() {
  NodeRangeScope scope(this);
  DCHECK_EQ(PeekToken(), ast::TokenKind::Default);
  auto& label = ConsumeToken();
  if (!CanPeekToken() || PeekToken() != ast::TokenKind::Colon)
    return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_EXPECT_COLON);
  return HandleLabeledStatement(label);
}

const ast::Node& Parser::ParseDoStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::Do);
  ConsumeToken();
  auto& statement = ParseStatement();
  if (!ConsumeTokenIf(ast::TokenKind::While))
    return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_EXPECT_WHILE);
  if (options_.disable_automatic_semicolon()) {
    auto& condition = ParseParenthesisExpression();
    ExpectSemicolon();
    return node_factory().NewDoStatement(GetSourceCodeRange(), statement,
                                         condition);
  }
  auto& condition = ParseParenthesisExpression();
  ConsumeTokenIf(ast::TokenKind::Semicolon);
  return node_factory().NewDoStatement(GetSourceCodeRange(), statement,
                                       condition);
}

const ast::Node& Parser::ParseExpressionStatement() {
  auto& expression = ParseExpression();
  if (expression == ast::SyntaxCode::Class ||
      expression == ast::SyntaxCode::Function) {
    return expression;
  }
  ExpectSemicolon();
  return node_factory().NewExpressionStatement(GetSourceCodeRange(),
                                               expression);
}

const ast::Node& Parser::ParseForStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::For);
  ConsumeToken();
  ExpectPunctuator(ast::TokenKind::LeftParenthesis,
                   ErrorCode::ERROR_STATEMENT_EXPECT_LPAREN);

  const auto* const document =
      CanPeekToken() && PeekToken() == ast::SyntaxCode::JsDocDocument
          ? &ConsumeToken()
          : nullptr;

  const auto& keyword = CanPeekToken() && ast::IsDeclarationKeyword(PeekToken())
                            ? ConsumeToken()
                            : NewEmptyName();

  if (keyword.Is<ast::Empty>()) {
    if (document)
      AddError(*document, ErrorCode::ERROR_STATEMENT_UNEXPECT_ANNOTATION);

    const auto& expression =
        CanPeekToken() && PeekToken() == ast::TokenKind::Semicolon
            ? NewElisionExpression()
            : ParseExpression();
    if (ConsumeTokenIf(ast::TokenKind::Semicolon)) {
      // 'for' '(' expression ';' condition ';' step ')' statement
      const auto& condition =
          CanPeekToken() && PeekToken() == ast::TokenKind::Semicolon
              ? NewElisionExpression()
              : ParseExpression();
      ExpectPunctuator(ast::TokenKind::Semicolon,
                       ErrorCode::ERROR_STATEMENT_EXPECT_SEMICOLON);
      const auto& step =
          CanPeekToken() && PeekToken() == ast::TokenKind::RightParenthesis
              ? NewElisionExpression()
              : ParseExpression();
      ExpectPunctuator(ast::TokenKind::RightParenthesis,
                       ErrorCode::ERROR_STATEMENT_EXPECT_RPAREN);
      const auto& statement = ParseStatement();
      return node_factory().NewForStatement(GetSourceCodeRange(), keyword,
                                            expression, condition, step,
                                            statement);
    }

    if (ConsumeTokenIf(ast::TokenKind::Of)) {
      // 'for' '(' binding 'of' expression ')' statement
      const auto& expression2 = ParseAssignmentExpression();
      ExpectPunctuator(ast::TokenKind::RightParenthesis,
                       ErrorCode::ERROR_STATEMENT_EXPECT_RPAREN);
      auto& body = ParseStatement();
      return node_factory().NewForOfStatement(GetSourceCodeRange(), keyword,
                                              expression, expression2, body);
    }

    if (ConsumeTokenIf(ast::TokenKind::RightParenthesis)) {
      if (!IsInExpression(expression))
        return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_INVALID);
      // 'for' '(' left_hand_side 'in' expression ')' statement
      const auto& left_hand_side =
          ast::BinaryExpression::LeftHandSideOf(expression);
      if (!IsLeftHandSide(left_hand_side))
        AddError(left_hand_side, ErrorCode::ERROR_STATEMENT_EXPECT_LHS);
      const auto& statement = ParseStatement();
      return node_factory().NewForInStatement(
          GetSourceCodeRange(), keyword, left_hand_side,
          ast::BinaryExpression::RightHandSideOf(expression), statement);
    }

    return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_INVALID);
  }

  const auto& binding = ParseBindingElement();
  if (!ast::IsValidForBinding(binding))
    return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_INVALID);
  if (ConsumeTokenIf(ast::TokenKind::In)) {
    // 'for' '(' keyword binding 'in' expression ')'
    const auto& initializer = BindingIntializerOf(binding);
    if (!initializer.Is<ast::ElisionExpression>())
      AddError(binding, ErrorCode::ERROR_STATEMENT_UNEXPECT_INITIALIZER);
    const auto& expression = ParseExpression();
    ExpectPunctuator(ast::TokenKind::RightParenthesis,
                     ErrorCode::ERROR_STATEMENT_EXPECT_RPAREN);
    const auto& statement = ParseStatement();
    return node_factory().NewForInStatement(GetSourceCodeRange(), keyword,
                                            binding, expression, statement);
  }
  if (ConsumeTokenIf(ast::TokenKind::Of)) {
    // 'for' '(' keyword binding 'of' expression ')'
    const auto& initializer = BindingIntializerOf(binding);
    if (!initializer.Is<ast::ElisionExpression>())
      AddError(binding, ErrorCode::ERROR_STATEMENT_UNEXPECT_INITIALIZER);
    const auto& expression = ParseExpression();
    ExpectPunctuator(ast::TokenKind::RightParenthesis,
                     ErrorCode::ERROR_STATEMENT_EXPECT_RPAREN);
    const auto& statement = ParseStatement();
    return node_factory().NewForOfStatement(GetSourceCodeRange(), keyword,
                                            binding, expression, statement);
  }
  if (ConsumeTokenIf(ast::TokenKind::Semicolon)) {
    // 'for' '(' keyword binding ';' condition? ';' step? ')
    const auto& condition =
        CanPeekToken() && PeekToken() == ast::TokenKind::Semicolon
            ? NewElisionExpression()
            : ParseExpression();
    ExpectPunctuator(ast::TokenKind::Semicolon,
                     ErrorCode::ERROR_STATEMENT_EXPECT_SEMICOLON);
    const auto& step =
        CanPeekToken() && PeekToken() == ast::TokenKind::RightParenthesis
            ? NewElisionExpression()
            : ParseExpression();
    ExpectPunctuator(ast::TokenKind::RightParenthesis,
                     ErrorCode::ERROR_STATEMENT_EXPECT_RPAREN);
    const auto& statement = ParseStatement();
    return node_factory().NewForStatement(GetSourceCodeRange(), keyword,
                                          binding, condition, step, statement);
  }
  ConsumeTokenIf(ast::TokenKind::RightParenthesis);
  return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_INVALID);
}

const ast::Node& Parser::ParseIfStatement() {
  auto& keyword = ConsumeToken();
  DCHECK_EQ(keyword, ast::TokenKind::If);
  auto& condition = ParseParenthesisExpression();
  auto& then_clause = ParseStatement();
  if (!ConsumeTokenIf(ast::TokenKind::Else)) {
    return node_factory().NewIfStatement(GetSourceCodeRange(), condition,
                                         then_clause);
  }
  auto& else_clause = ParseStatement();
  return node_factory().NewIfElseStatement(GetSourceCodeRange(), condition,
                                           then_clause, else_clause);
}

const ast::Node& Parser::ParseKeywordStatement() {
  NodeRangeScope scope(this);
  const auto& keyword = PeekToken();
  DCHECK(ast::Name::IsKeyword(keyword)) << keyword;
  switch (ast::Name::KindOf(keyword)) {
    case ast::TokenKind::Async:
      ConsumeToken();
      if (CanPeekToken() && PeekToken() == ast::TokenKind::Function)
        return ParseFunction(ast::FunctionKind::Async);
      PushBackToken(keyword);
      break;
    case ast::TokenKind::Break:
      return ParseBreakStatement();
    case ast::TokenKind::Case:
      return ParseCaseClause();
    case ast::TokenKind::Class:
      return ParseClass();
    case ast::TokenKind::Continue:
      return ParseContinueStatement();
    case ast::TokenKind::Const:
      return ParseConstStatement();
    case ast::TokenKind::Debugger: {
      auto& expression = node_factory().NewReferenceExpression(ConsumeToken());
      ExpectSemicolon();
      return node_factory().NewExpressionStatement(GetSourceCodeRange(),
                                                   expression);
    }
    case ast::TokenKind::Default:
      return ParseDefaultLabel();
    case ast::TokenKind::Do:
      return ParseDoStatement();
    case ast::TokenKind::For:
      return ParseForStatement();
    case ast::TokenKind::Function:
      ConsumeToken();
      if (ConsumeTokenIf(ast::TokenKind::Times))
        return ParseFunction(ast::FunctionKind::Generator);
      return ParseFunction(ast::FunctionKind::Normal);
    case ast::TokenKind::If:
      return ParseIfStatement();
    case ast::TokenKind::Let:
      return ParseLetStatement();
    case ast::TokenKind::Return:
      return ParseReturnStatement();
    case ast::TokenKind::Switch:
      return ParseSwitchStatement();
    case ast::TokenKind::Throw:
      return ParseThrowStatement();
    case ast::TokenKind::Try:
      return ParseTryStatement();
    case ast::TokenKind::Var:
      return ParseVarStatement();
    case ast::TokenKind::While:
      return ParseWhileStatement();

    case ast::TokenKind::With:
      return ParseWithStatement();

    // Reserved keywords
    case ast::TokenKind::Catch:
    case ast::TokenKind::Else:
    case ast::TokenKind::Enum:
    case ast::TokenKind::Finally:
    case ast::TokenKind::Interface:
    case ast::TokenKind::Package:
    case ast::TokenKind::Private:
    case ast::TokenKind::Protected:
    case ast::TokenKind::Public:
    case ast::TokenKind::Static:
      ConsumeToken();
      return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_RESERVED_WORD);
    default:
      // An expression starts with keyword.
      break;
  }
  return ParseExpressionStatement();
}

const ast::Node& Parser::ParseLetStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::Let);
  ConsumeToken();
  const auto& elements = ParseBindingElements();
  ExpectSemicolon();
  return node_factory().NewLetStatement(GetSourceCodeRange(), elements);
}

const ast::Node& Parser::ParseNameAsStatement() {
  auto& name = PeekToken();
  if (ast::Name::IsKeyword(name))
    return ParseKeywordStatement();
  ConsumeToken();
  if (!CanPeekToken()) {
    PushBackToken(name);
    return ParseExpressionStatement();
  }
  if (is_separated_by_newline_) {
    if (options_.disable_automatic_semicolon())
      AddError(ErrorCode::ERROR_STATEMENT_UNEXPECT_NEWLINE);
  } else {
    if (CanPeekToken() && PeekToken() == ast::TokenKind::Colon)
      return HandleLabeledStatement(name);
  }
  PushBackToken(name);
  return ParseExpressionStatement();
}

// Yet another entry point called by statement parser.
const ast::Node& Parser::ParseParenthesisExpression() {
  if (!ConsumeTokenIf(ast::TokenKind::LeftParenthesis)) {
    // Some people think it is redundant that C++ statement requires
    // parenthesis for an expression after keyword.
    AddError(ErrorCode::ERROR_STATEMENT_EXPECT_LPAREN);
    return ParseExpression();
  }
  auto& expression = ParseExpression();
  ExpectPunctuator(ast::TokenKind::RightParenthesis,
                   ErrorCode::ERROR_STATEMENT_EXPECT_RPAREN);
  return expression;
}

const ast::Node& Parser::ParseReturnStatement() {
  ConsumeToken();
  if (is_separated_by_newline_) {
    if (options_.disable_automatic_semicolon())
      AddError(ErrorCode::ERROR_STATEMENT_UNEXPECT_NEWLINE);
    return node_factory().NewReturnStatement(GetSourceCodeRange(),
                                             NewElisionExpression());
  } else {
    if (CanPeekToken() && PeekToken() == ast::TokenKind::RightBrace) {
      return node_factory().NewReturnStatement(GetSourceCodeRange(),
                                               NewElisionExpression());
    }
  }
  if (CanPeekToken() && ConsumeTokenIf(ast::TokenKind::Semicolon)) {
    return node_factory().NewReturnStatement(GetSourceCodeRange(),
                                             NewElisionExpression());
  }
  auto& expression = ParseExpression();
  ExpectSemicolon();
  return node_factory().NewReturnStatement(GetSourceCodeRange(), expression);
}

// The entry point
const ast::Node& Parser::ParseStatement() {
  if (!CanPeekToken())
    return NewEmptyStatement(source_code().end());
  NodeRangeScope scope(this);
  const auto& token = PeekToken();
  if (token == ast::SyntaxCode::JsDocDocument)
    return ParseJsDocAsStatement();
  if (token == ast::SyntaxCode::Name)
    return ParseNameAsStatement();
  if (token.is_literal())
    return ParseExpressionStatement();
  if (token == ast::TokenKind::LeftBrace)
    return ParseBlockStatement();
  if (token == ast::TokenKind::Semicolon) {
    auto& statement = NewEmptyStatement(
        SourceCodeRange::CollapseToStart(PeekToken().range()));
    ConsumeToken();
    return statement;
  }
  return ParseExpressionStatement();
}

const ast::Node& Parser::ParseSwitchStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::Switch);
  ConsumeToken();
  auto& expression = ParseParenthesisExpression();
  std::vector<const ast::Node*> clauses;
  if (!ConsumeTokenIf(ast::TokenKind::LeftBrace)) {
    AddError(ErrorCode::ERROR_STATEMENT_EXPECT_LBRACE);
    return node_factory().NewSwitchStatement(GetSourceCodeRange(), expression,
                                             clauses);
  }
  while (CanPeekToken()) {
    if (ConsumeTokenIf(ast::TokenKind::RightBrace))
      break;
    clauses.push_back(&ParseStatement());
  }
  return node_factory().NewSwitchStatement(GetSourceCodeRange(), expression,
                                           clauses);
}

const ast::Node& Parser::ParseThrowStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::Throw);
  ConsumeToken();
  auto& expression = ParseExpression();
  ExpectSemicolon();
  return node_factory().NewThrowStatement(GetSourceCodeRange(), expression);
}

const ast::Node& Parser::ParseTryStatement() {
  ConsumeToken();
  auto& try_block = ParseStatement();
  if (ConsumeTokenIf(ast::TokenKind::Finally)) {
    auto& finally_block = ParseBlockStatement();
    return node_factory().NewTryFinallyStatement(GetSourceCodeRange(),
                                                 try_block, finally_block);
  }
  if (!CanPeekToken())
    return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_EXPECT_CATCH);
  const auto& catch_token = PeekToken();
  if (!ConsumeTokenIf(ast::TokenKind::Catch))
    return NewInvalidStatement(ErrorCode::ERROR_STATEMENT_EXPECT_CATCH);
  ExpectPunctuator(ast::TokenKind::LeftParenthesis,
                   ErrorCode::ERROR_STATEMENT_EXPECT_LPAREN);
  const auto& catch_parameter = ParseBindingElement();
  if (HasInitializer(catch_parameter))
    AddError(catch_parameter, ErrorCode::ERROR_STATEMENT_UNEXPECT_INITIALIZER);
  ConsumeTokenIf(ast::TokenKind::RightParenthesis);
  const auto& catch_block = ParseStatement();
  const auto& catch_clause = node_factory().NewCatchClause(
      SourceCodeRange::Merge(catch_token.range(), catch_block.range()),
      catch_parameter, catch_block);
  if (!ConsumeTokenIf(ast::TokenKind::Finally)) {
    return node_factory().NewTryCatchStatement(GetSourceCodeRange(), try_block,
                                               catch_clause);
  }
  auto& finally_block = ParseStatement();
  return node_factory().NewTryCatchFinallyStatement(
      GetSourceCodeRange(), try_block, catch_clause, finally_block);
}

const ast::Node& Parser::ParseVarStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::Var);
  ConsumeToken();
  const auto& elements = ParseBindingElements();
  ExpectSemicolon();
  return node_factory().NewVarStatement(GetSourceCodeRange(), elements);
}

const ast::Node& Parser::ParseWhileStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::While);
  ConsumeToken();
  auto& condition = ParseParenthesisExpression();
  auto& statement = ParseStatement();
  return node_factory().NewWhileStatement(GetSourceCodeRange(), condition,
                                          statement);
}

const ast::Node& Parser::ParseWithStatement() {
  DCHECK_EQ(PeekToken(), ast::TokenKind::With);
  ConsumeToken();
  auto& expression = ParseParenthesisExpression();
  auto& statement = ParseStatement();
  return node_factory().NewWithStatement(GetSourceCodeRange(), expression,
                                         statement);
}

}  // namespace parser
}  // namespace aoba
