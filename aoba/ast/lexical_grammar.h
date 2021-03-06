// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_AST_LEXICAL_GRAMMAR_H_
#define AOBA_AST_LEXICAL_GRAMMAR_H_

namespace aoba {

#define FOR_EACH_CHARACTER_NAME(V)                   \
  V(0x0009, Tab, TAB)                                \
  V(0x000A, LineFeed, LINE_FEED)                     \
  V(0x000B, LineTab, LINE_TAB)                       \
  V(0x000C, FormFeed, FORM_FEED)                     \
  V(0x000D, CarriageReturn, CARRIAGE_RETURN)         \
  V(0x0020, Space, SPACE)                            \
  V(0x00A0, NonBreakSpace, NON_BREAK_SPACE)          \
  V(0x2028, LineSeparator, LINE_SEPARATOR)           \
  V(0x2029, ParagraphSeparator, PARAGRAPH_SEPARATOR) \
  V(0xFEFF, ZeroWidthNoBreakSpace, ZERO_WIDT_HNO_BREAK_SPACE)

#define FOR_EACH_JAVASCRIPT_TOKEN(V)        \
  V(IdentifierName, IDENTIFIER_NAME)        \
  V(MultiLineComment, MULTI_LINE_COMMENT)   \
  V(SingleLineComment, SINGLE_LINE_COMMENT) \
  V(Punctuator, PUNCTUATOR)                 \
  V(NumericLiteral, NUMERIC_LITERAL)        \
  V(StringLiteral, STRING_LITERAL)          \
  V(Template, TEMPLATE)

// Note: true, false and null are not actual keywords.
#define FOR_EACH_JAVASCRIPT_KEYWORD(V)   \
  V(await, Await, AWAIT)                 \
  V(break, Break, BREAK)                 \
  V(case, Case, CASE)                    \
  V(catch, Catch, CATCH)                 \
  V(class, Class, CLASS)                 \
  V(const, Const, CONST)                 \
  V(continue, Continue, CONTINUE)        \
  V(debugger, Debugger, DEBUGGER)        \
  V(default, Default, DEFAULT)           \
  V(delete, Delete, DELETE)              \
  V(do, Do, DO)                          \
  V(else, Else, ELSE)                    \
  V(enum, Enum, ENUM)                    \
  V(export, Export, EXPORT)              \
  V(extends, Extends, EXTENDS)           \
  V(false, False, FALSE)                 \
  V(finally, Finally, FINALLY)           \
  V(for, For, FOR)                       \
  V(function, Function, FUNCTION)        \
  V(if, If, IF)                          \
  V(implements, Implements, IMPLEMENTS)  \
  V(import, Import, IMPORT)              \
  V(in, In, IN)                          \
  V(instanceof, InstanceOf, INSTANCE_OF) \
  V(interface, Interface, INTERFACE)     \
  V(let, Let, LET)                       \
  V(new, New, NEW)                       \
  V(null, Null, NULL)                    \
  V(package, Package, PACKAGE)           \
  V(private, Private, PRIVATE)           \
  V(protected, Protected, PROTECTED)     \
  V(public, Public, PUBLIC)              \
  V(return, Return, RETURN)              \
  V(static, Static, STATIC)              \
  V(super, Super, SUPER)                 \
  V(switch, Switch, SWITCH)              \
  V(this, This, THIS)                    \
  V(throw, Throw, THROW)                 \
  V(true, True, TRUE)                    \
  V(try, Try, TRY)                       \
  V(typeof, TypeOf, TYPE_OF)             \
  V(var, Var, VAR)                       \
  V(void, Void, VOID)                    \
  V(while, While, WHILE)                 \
  V(with, With, WITH)                    \
  V(yield, Yield, YIELD)                 \
  V(yield_star, YieldStar, YIELD_STAR)

#define FOR_EACH_JAVASCRIPT_KNOWN_WORD(V)                         \
  V(Array, Array, ARRAY)                                          \
  V(arguments, Arguments, ARGUMENTS)                              \
  V(async, Async, ASYNC)                                          \
  V(boolean, Boolean, BOOLEAN)                                    \
  V(constructor, Constructor, CONSTRUCTOR)                        \
  V(from, From, CONSTRUCTOR)                                      \
  V(get, Get, GET)                                                \
  V(global, Global, Global)                                       \
  V(hasInstance, HasInstance, HAS_INSTANCE)                       \
  V(isConcatSpreadable, IsConcatSpreadable, IS_concat_spreadable) \
  V(iterator, Iterator, ITERATOR)                                 \
  V(match, Match, Match)                                          \
  V(number, Number, NUMBER)                                       \
  V(Object, Object, OBJECT)                                       \
  V(of, Of, OF)                                                   \
  V(prototype, Prototype, PROTOTYPE)                              \
  V(replace, Replace, REPLACE)                                    \
  V(search, Search, SEARCH)                                       \
  V(set, Set, SET)                                                \
  V(species, Species, SPECIES)                                    \
  V(split, Split, SPLIT)                                          \
  V(string, String, String) /* NOLINT */                          \
  V(Symbol, SymbolObject, SYMBOL_OBJECT)                          \
  V(symbol, Symbol, SYMBOL)                                       \
  /** new.target */                                               \
  V(target, Target, Target)                                       \
  V(toPrimitive, ToPrimitive, TO_PRIMITIVE)                       \
  V(toStringTag, ToStringTag, TO_STRINGTAG)                       \
  V(unscopables, Unscopables, unscopables)                        \
  V(undefined, Undefined, UNDEFINED)

// Visitor parameter takes:
//  - String representation of punctuator.
//  - Capitalized name
//  - Upper case
//  - Binary operator category
// Note: Parser generates PostPlusPlus and PostMinusMinus tokens instead of
// Lexer.
#define FOR_EACH_JAVASCRIPT_PUNCTUATOR(V)                                   \
  V("???", Invalid, INVALID, None)                                          \
  V("{", LeftBrace, LEFT_BRACE, None)                                       \
  V("}", RightBrace, RIGHT_BRACE, None)                                     \
  V("[", LeftBracket, LEFT_BRACKET, None)                                   \
  V("]", RightBracket, RIGHT_BRACKET, None)                                 \
  V("(", LeftParenthesis, LEFT_PARENTHESIS, None)                           \
  V(")", RightParenthesis, RIGHT_PARENTHESIS, None)                         \
  V(".", Dot, DOT, None)                                                    \
  V("..", DotDot, DOT_DOT, None)                                            \
  V("...", DotDotDot, DOT_DOT_DOT, None)                                    \
  V(";", Semicolon, SEMICOLON, None)                                        \
  V(",", Comma, COMMA, None)                                                \
  V("<", LessThan, LEFT_THAN, Relational)                                   \
  V("<=", LessThanOrEqual, LESS_THAN_OR_EQUAL, Relational)                  \
  V(">", GreaterThan, GREATER_THAN, Relational)                             \
  V(">=", GreaterThanOrEqual, GREATER_THAN_OR_EQUAL, Relational)            \
  V("==", EqualEqual, EQUAL_EQUAL, Equality)                                \
  V("===", EqualEqualEqual, EQUAL_EQUAL_EQUAL, Equality)                    \
  V("+", Plus, PLUS, Additive)                                              \
  V("-", Minus, MIUS, Additive)                                             \
  V("*", Times, TIMES, Multiplicative)                                      \
  V("%", Modulo, MODULO, Multiplicative)                                    \
  V("++", PlusPlus, PLUS_PLUS, None)                                        \
  V("++", PostPlusPlus, POST_PLUS_PLUS, None)                               \
  V("--", MinusMinus, MINUS_MINUS, None)                                    \
  V("--", PostMinusMinus, POST_MINUS_MINUS, None)                           \
  V("<<", LeftShift, LEFT_SHIFT, Shift)                                     \
  V(">>", RightShift, RIGHT_SHIFT, Shift)                                   \
  V(">>>", UnsignedRightShift, UNSIGNED_RIGHT_SHIFT, Shift)                 \
  V("&", BitAnd, BIT_AND, BitwiseAnd)                                       \
  V("|", BitOr, BIT_OR, BitwiseOr)                                          \
  V("^", BitXor, BIT_XOR, BitwiseXor)                                       \
  V("!", LogicalNot, LOGICAL_NOT, None)                                     \
  V("!=", NotEqual, NOT_EQUAL, Equality)                                    \
  V("!==", NotEqualEqual, NOT_EQUAL_EQUAL, Equality)                        \
  V("~", BitNot, BIT_NOT, None)                                             \
  V("&&", LogicalAnd, LOGICAL_AND, LogicalAnd)                              \
  V("||", LogicalOr, LOGICAL_OR, LogicalOr)                                 \
  V("?", Question, QUESTION, None)                                          \
  V(":", Colon, COLON, None)                                                \
  V("=", Equal, EQUAL, Assignment)                                          \
  V("+=", PlusEqual, PLUS_EQUAL, Assignment)                                \
  V("-=", MinusEqual, PLUS_EQUAL, Assignment)                               \
  V("*=", TimesEqual, TIMES_EQUAL, Assignment)                              \
  V("%=", ModuloEqual, MODULO_EQUAL, Assignment)                            \
  V("<<=", LeftShiftEqual, LEFT_SHIT_EQUAL, Assignment)                     \
  V(">>=", RightShiftEqual, RIGHT_SHIT_EQUAL, Assignment)                   \
  V(">>>=", UnsignedRightShiftEqual, UNSIGNED_RIGHT_SHIT_EQUAL, Assignment) \
  V("&=", BitAndEqual, BIT_AND_EQUAL, Assignment)                           \
  V("|=", BitOrEqual, BIT_OR_EQUAL, Assignment)                             \
  V("^=", BitXorEqual, BIT_XOR_EQUAL, Assignment)                           \
  V("=>", Arrow, ARROW, None)                                               \
  V("**", TimesTimes, TIMES_TIMES, Exponentiation)                          \
  V("**=", TimesTimesEqual, TIMES_TIMES_EQUAL, Assignment)                  \
  V("/", Divide, DIVIDE, Multiplicative)                                    \
  V("/=", DivideEqual, DIVIDE_EQUAL, Assignment)

}  // namespace aoba

#endif  // AOBA_AST_LEXICAL_GRAMMAR_H_
