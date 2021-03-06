// Copyright (c) 2017 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sstream>
#include <string>

#include "aoba/analyzer/type_resolver.h"

#include "aoba/analyzer/analyzer_test_base.h"
#include "aoba/analyzer/context.h"
#include "aoba/analyzer/factory.h"
#include "aoba/analyzer/name_resolver.h"
#include "aoba/analyzer/print_as_tree.h"
#include "aoba/analyzer/properties_editor.h"
#include "aoba/analyzer/type.h"
#include "aoba/analyzer/value_editor.h"
#include "aoba/analyzer/values.h"
#include "aoba/ast/node.h"
#include "aoba/ast/node_traversal.h"
#include "aoba/ast/syntax.h"
#include "aoba/ast/tokens.h"
#include "aoba/base/escaped_string_piece.h"
#include "aoba/parser/public/parse.h"
#include "aoba/testing/simple_error_sink.h"

namespace aoba {
namespace analyzer {

namespace {

bool ShouldPrintBaseClasses(const Class& class_value) {
  if (class_value.base_classes().empty())
    return false;
  if (!class_value.is_class())
    return true;
  if (class_value.base_classes().size() != 1)
    return true;
  const auto& base_class = *class_value.base_classes().begin();
  return !base_class.base_classes().empty();
}

}  // namespace

//
// TypeResolverTest
//
class TypeResolverTest : public AnalyzerTestBase {
 protected:
  TypeResolverTest() = default;
  ~TypeResolverTest() override = default;

  std::string RunOn(base::StringPiece script_text);

 private:
  DISALLOW_COPY_AND_ASSIGN(TypeResolverTest);
};

std::string TypeResolverTest::RunOn(base::StringPiece script_text) {
  const auto& module = ParseAsModule(script_text);
  const auto& context = NewContext();
  context->ResetCurrentIdForTesting(500);
  context->InstallClass(ast::TokenKind::Array);
  context->InstallClass(ast::TokenKind::Object);
  context->ResetCurrentIdForTesting(600);
  {
    NameResolver name_resolver(context.get());
    name_resolver.RunOn(module);
  }
  {
    TypeResolver type_resolver(context.get());
    type_resolver.PrepareForTesting();
    static_cast<Pass&>(type_resolver).RunOn(module);
    static_cast<Pass&>(type_resolver).RunOnAll();
  }
  std::ostringstream ostream;
  ostream << AsPrintableTree(*context, module) << std::endl;
  // Dump class base classes
  for (const auto& node : ast::NodeTraversal::DescendantsOf(module)) {
    const auto* value = context->TryValueOf(node);
    if (!value || !value->Is<Class>())
      continue;
    const auto& class_value = value->As<Class>();
    if (!ShouldPrintBaseClasses(class_value))
      continue;
    ostream << class_value << std::endl;
    ostream << "+--BaseClasses" << std::endl;
    for (const auto& base_class : class_value.base_classes())
      ostream << "|   +--" << base_class << std::endl;
  }
  // Dump errors
  for (const auto* error : error_sink().errors())
    ostream << error << std::endl;
  return ostream.str();
}

TEST_F(TypeResolverTest, AnyType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--AnyType |*|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {*}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {*} */ var foo;"));
}

TEST_F(TypeResolverTest, BaseClass) {
  EXPECT_EQ(
      "Module\n"
      "+--Class Class[Base@1002]\n"
      "|  +--Name |Base|\n"
      "|  +--ElisionExpression ||\n"
      "|  +--ObjectInitializer |{}|\n"
      "+--Class Class[Derived@1006]\n"
      "|  +--Name |Derived|\n"
      "|  +--ReferenceExpression ClassVar[Base@1001]\n"
      "|  |  +--Name |Base|\n"
      "|  +--ObjectInitializer |{}|\n"
      "Class[Derived@1006]\n"
      "+--BaseClasses\n"
      "|   +--Class[Base@1002]\n",
      RunOn("class Base {}\n"
            "class Derived extends Base {}"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@interface|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] {Foo@1001}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression || Interface[Foo@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@interface|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Bar@1005] {Bar@1002}\n"
      "|  |  |  +--Name |Bar|\n"
      "|  |  |  +--ElisionExpression || Interface[Bar@1006]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@implements|\n"
      "|  |  |  +--TypeName {Foo@1001}\n"
      "|  |  |  |  +--Name |Foo|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@implements|\n"
      "|  |  |  +--TypeName {Bar@1002}\n"
      "|  |  |  |  +--Name |Bar|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--Class Class[Baz@1010]\n"
      "|  |  +--Name |Baz|\n"
      "|  |  +--ElisionExpression ||\n"
      "|  |  +--ObjectInitializer |{}|\n"
      "Class[Baz@1010]\n"
      "+--BaseClasses\n"
      "|   +--Class[Object@504]\n"
      "|   +--Interface[Foo@1002]\n"
      "|   +--Interface[Bar@1006]\n",
      RunOn("/** @interface */ var Foo;\n"
            "/** @interface */ var Bar;\n"
            "/** @implements {Foo} @implements {Bar} */ class Baz {}"));
}

TEST_F(TypeResolverTest, BaseClassInQualifiedName) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] "
      "{function(new:Foo@1001):Foo@1001}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression || Class[Foo@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@extends|\n"
      "|  |  |  +--TypeName {Foo@1001}\n"
      "|  |  |  |  +--Name |Foo|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--Declaration\n"
      "|  |  +--MemberExpression PublicProperty[Bar@1005]\n"
      "|  |  |  +--ReferenceExpression VarVar[Foo@1001]\n"
      "|  |  |  |  +--Name |Foo|\n"
      "|  |  |  +--Name |Bar|\n"
      "|  |  +--ElisionExpression || Class[Foo.Bar@1006]\n",
      RunOn("/** @constructor */ var Foo;\n"
            "/** @constructor @extends {Foo} */ Foo.Bar;"));
}

TEST_F(TypeResolverTest, BaseClassError) {
  EXPECT_EQ(
      "Module\n"
      "+--Class Class[Foo@1002]\n"
      "|  +--Name |Foo|\n"
      "|  +--NumericLiteral |1|\n"
      "|  +--ObjectInitializer |{}|\n"
      "ANALYZER_ERROR_TYPE_RESOLVER_EXPECT_CLASS@18:19\n",
      RunOn("class Foo extends 1 {}"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@interface|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[A@1001] {A@1001}\n"
      "|  |  |  +--Name |A|\n"
      "|  |  |  +--ElisionExpression || Interface[A@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@implements|\n"
      "|  |  |  +--TypeName {A@1001}\n"
      "|  |  |  |  +--Name |A|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@implements|\n"
      "|  |  |  +--TypeName {A@1001}\n"
      "|  |  |  |  +--Name |A|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--Class Class[Bar@1006]\n"
      "|  |  +--Name |Bar|\n"
      "|  |  +--ElisionExpression ||\n"
      "|  |  +--ObjectInitializer |{}|\n"
      "Class[Bar@1006]\n"
      "+--BaseClasses\n"
      "|   +--Class[Object@504]\n"
      "|   +--Interface[A@1002]\n"
      "ANALYZER_ERROR_TYPE_RESOLVER_MULTIPLE_OCCURRENCES@42:60\n",
      RunOn("/** @interface */ var A;\n"
            "/** @implements {A} @implements {A} */ class Bar {}"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[A@1001] {function(new:"
      "A@1001):A@1001}\n"
      "|  |  |  +--Name |A|\n"
      "|  |  |  +--ElisionExpression || Class[A@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@extends|\n"
      "|  |  |  +--TypeName {A@1001}\n"
      "|  |  |  |  +--Name |A|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--Class Class[B@1006]\n"
      "|  |  +--Name |B|\n"
      "|  |  +--ElisionExpression ||\n"
      "|  |  +--ObjectInitializer |{}|\n"
      "ANALYZER_ERROR_TYPE_RESOLVER_UNEXPECT_EXTENDS@31:57\n",
      RunOn("/** @constructor */ var A;\n"
            "/** @extends {A} */ class B {}"))
      << "class declaration can not have @extends";
}

TEST_F(TypeResolverTest, Constructor) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] "
      "{function(new:Foo@1001):Foo@1001}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression || Class[Foo@1002]\n",
      RunOn("/** @constructor */ var Foo;"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] "
      "{function(new:Foo@1001):Foo@1001}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--Function<Normal> Class[Foo@1003]\n"
      "|  |  |  |  +--Empty ||\n"
      "|  |  |  |  +--ParameterList |()|\n"
      "|  |  |  |  +--BlockStatement |{}|\n",
      RunOn("/** @constructor */ var Foo = function() {};"));
}

TEST_F(TypeResolverTest, ConstructorBaseClass) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] {function(new:"
      "Foo@1001):Foo@1001}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression || Class[Foo@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@extends|\n"
      "|  |  |  +--TypeName {Foo@1001}\n"
      "|  |  |  |  +--Name |Foo|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--Function<Normal> Class[Bar@1007]\n"
      "|  |  +--Name |Bar|\n"
      "|  |  +--ParameterList |()|\n"
      "|  |  +--BlockStatement |{}|\n"
      "Class[Bar@1007]\n"
      "+--BaseClasses\n"
      "|   +--Class[Foo@1002]\n",
      RunOn("/** @constructor */ var Foo;\n"
            "/** @constructor @extends {Foo} */ function Bar() {}"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] {function(new:"
      "Foo@1001):Foo@1001}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression || Class[Foo@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@extends|\n"
      "|  |  |  +--TypeName {Foo@1001}\n"
      "|  |  |  |  +--Name |Foo|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Bar@1005] {function(new:"
      "Bar@1002):Bar@1002}\n"
      "|  |  |  +--Name |Bar|\n"
      "|  |  |  +--ElisionExpression || Class[Bar@1006]\n"
      "Class[Bar@1006]\n"
      "+--BaseClasses\n"
      "|   +--Class[Foo@1002]\n",
      RunOn("/** @constructor */ var Foo;\n"
            "/** @constructor @extends {Foo} */ var Bar;"));
}

TEST_F(TypeResolverTest, Function) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@param|\n"
      "|  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  +--Name |number|\n"
      "|  |  |  +--ReferenceExpression ParameterVar[x@1003]\n"
      "|  |  |  |  +--Name |x|\n"
      "|  |  |  +--JsDocText ||\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@return|\n"
      "|  |  |  +--TypeName {%string%}\n"
      "|  |  |  |  +--Name |string|\n"
      "|  |  |  +--JsDocText |*/|\n"
      "|  +--Function<Normal> Function[foo@1001] "
      "{function(%number%):%string%}\n"
      "|  |  +--Name |foo|\n"
      "|  |  +--ParameterList\n"
      "|  |  |  +--BindingNameElement ParameterVar[x@1003]\n"
      "|  |  |  |  +--Name |x|\n"
      "|  |  |  |  +--ElisionExpression ||\n"
      "|  |  +--BlockStatement |{}|\n",
      RunOn("/** @param {number} x @return {string} */ function foo(x) {}"));
}

TEST_F(TypeResolverTest, FunctionInterface) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@interface|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@template|\n"
      "|  |  |  +--TypeName {T@1001}\n"
      "|  |  |  |  +--Name |T|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--Function<Normal> GenericInterface<T>[Foo@1003]\n"
      "|  |  +--Name |Foo|\n"
      "|  |  +--ParameterList |()|\n"
      "|  |  +--BlockStatement |{}|\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@interface|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@template|\n"
      "|  |  |  +--TypeName {T@1003}\n"
      "|  |  |  |  +--Name |T|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--Function<Normal> GenericInterface<T>[Bar@1008]\n"
      "|  |  +--Name |Bar|\n"
      "|  |  +--ParameterList |()|\n"
      "|  |  +--BlockStatement |{}|\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**\\n"
      " *|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@interface|\n"
      "|  |  +--JsDocText |\\n"
      " *|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@template|\n"
      "|  |  |  +--TypeName {U@1005}\n"
      "|  |  |  |  +--Name |U|\n"
      "|  |  +--JsDocText |\\n"
      " *|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@extends|\n"
      "|  |  |  +--TypeApplication\n"
      "|  |  |  |  +--TypeName {Foo<T>@1002}\n"
      "|  |  |  |  |  +--Name |Foo|\n"
      "|  |  |  |  +--Tuple\n"
      "|  |  |  |  |  +--TypeName {U@1005}\n"
      "|  |  |  |  |  |  +--Name |U|\n"
      "|  |  +--JsDocText |\\n"
      " *|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@extends|\n"
      "|  |  |  +--TypeApplication\n"
      "|  |  |  |  +--TypeName {Bar<T>@1004}\n"
      "|  |  |  |  |  +--Name |Bar|\n"
      "|  |  |  |  +--Tuple\n"
      "|  |  |  |  |  +--TypeName {U@1005}\n"
      "|  |  |  |  |  |  +--Name |U|\n"
      "|  |  +--JsDocText |\\n"
      " */|\n"
      "|  +--Function<Normal> GenericInterface<U>[Baz@1013]\n"
      "|  |  +--Name |Baz|\n"
      "|  |  +--ParameterList |()|\n"
      "|  |  +--BlockStatement |{}|\n"
      "GenericInterface<U>[Baz@1013]\n"
      "+--BaseClasses\n"
      "|   +--ConstructedInterface<U@1005>[Foo@1016]\n"
      "|   +--ConstructedInterface<U@1005>[Bar@1017]\n",
      RunOn("/** @interface @template T */ function Foo() {}\n"
            "/** @interface @template T */ function Bar() {}\n"
            "/**\n"
            " * @interface\n"
            " * @template U\n"
            " * @extends {Foo<U>}\n"
            " * @extends {Bar<U>}\n"
            " */\n"
            "function Baz() {}"));
}

TEST_F(TypeResolverTest, InvalidType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--InvalidType |+|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {?}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {+} */ var foo"));
}

TEST_F(TypeResolverTest, NonNullableType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--NonNullableType\n"
      "|  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  +--Name |number|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {%number%}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n"
      "ANALYZER_ERROR_JSDOC_EXPECT_NULLABLE_TYPE@12:18\n",
      RunOn("/** @type {!number} */ var foo"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] "
      "{function(new:Foo@1001):Foo@1001}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression || Class[Foo@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--NonNullableType\n"
      "|  |  |  |  +--TypeName {Foo@1001}\n"
      "|  |  |  |  |  +--Name |Foo|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[bar@1005] {Foo@1001}\n"
      "|  |  |  +--Name |bar|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @constructor */ var Foo;\n"
            "/** @type {!Foo} */ var bar;\n"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--NonNullableType\n"
      "|  |  |  |  +--FunctionType<Normal>\n"
      "|  |  |  |  |  +--Tuple |()|\n"
      "|  |  |  |  |  +--VoidType ||\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {function(this:?)}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n"
      "ANALYZER_ERROR_JSDOC_EXPECT_NULLABLE_TYPE@12:22\n",
      RunOn("/** @type {!function()} */ var foo"));
}

TEST_F(TypeResolverTest, NullableType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--NullableType\n"
      "|  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  +--Name |number|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {%null%|%number%}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {?number} */ var foo"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] "
      "{function(new:Foo@1001):Foo@1001}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression || Class[Foo@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--TypeName {Foo@1001}\n"
      "|  |  |  |  +--Name |Foo|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[bar@1005] {%null%|Foo@1001}\n"
      "|  |  |  +--Name |bar|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @constructor */ var Foo;\n"
            "/** @type {Foo} */ var bar;\n"))
      << "Non-primitive type reference is nullable";
}

TEST_F(TypeResolverTest, OptionalType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@param|\n"
      "|  |  |  +--OptionalType\n"
      "|  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  +--Name |number|\n"
      "|  |  |  +--ReferenceExpression ParameterVar[opt_x@1002]\n"
      "|  |  |  |  +--Name |opt_x|\n"
      "|  |  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {function(%number%)}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression || Function[foo@1003]\n",
      RunOn("/** @param {number=} opt_x */ var foo"));
}

TEST_F(TypeResolverTest, RecordType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--RecordType\n"
      "|  |  |  |  +--Property\n"
      "|  |  |  |  |  +--Name |foo|\n"
      "|  |  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  |  +--Name |number|\n"
      "|  |  |  |  +--Name |baz|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[quux@1001] "
      "{%null%|{foo:%number%,baz:*}}\n"
      "|  |  |  +--Name |quux|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {{foo: number, baz}} */ var quux;"));
}

TEST_F(TypeResolverTest, RecordTypeError) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--RecordType\n"
      "|  |  |  |  +--Property\n"
      "|  |  |  |  |  +--Name |foo|\n"
      "|  |  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  |  +--Name |number|\n"
      "|  |  |  |  +--Property\n"
      "|  |  |  |  |  +--Name |foo|\n"
      "|  |  |  |  |  +--TypeName {%string%}\n"
      "|  |  |  |  |  |  +--Name |string|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[quux@1001] {%null%|{foo:%number%}}\n"
      "|  |  |  +--Name |quux|\n"
      "|  |  |  +--ElisionExpression ||\n"
      "ANALYZER_ERROR_JSDOC_MULTIPLE_PROPERTY@12:28\n",
      RunOn("/** @type {{foo: number, foo: string}} */ var quux;"));
}

TEST_F(TypeResolverTest, RestType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@param|\n"
      "|  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  +--Name |number|\n"
      "|  |  |  +--ReferenceExpression ParameterVar[level@1003]\n"
      "|  |  |  |  +--Name |level|\n"
      "|  |  |  +--JsDocText ||\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@param|\n"
      "|  |  |  +--RestType\n"
      "|  |  |  |  +--TypeName {%string%}\n"
      "|  |  |  |  |  +--Name |string|\n"
      "|  |  |  +--ReferenceExpression ParameterVar[args@1004]\n"
      "|  |  |  |  +--Name |args|\n"
      "|  |  |  +--JsDocText |*/|\n"
      "|  +--Function<Normal> Function[foo@1001] "
      "{function(%number%,Array<%string%>@1001)}\n"
      "|  |  +--Name |foo|\n"
      "|  |  +--ParameterList\n"
      "|  |  |  +--BindingNameElement ParameterVar[level@1003]\n"
      "|  |  |  |  +--Name |level|\n"
      "|  |  |  |  +--ElisionExpression ||\n"
      "|  |  |  +--BindingRestElement\n"
      "|  |  |  |  +--BindingNameElement ParameterVar[args@1004]\n"
      "|  |  |  |  |  +--Name |args|\n"
      "|  |  |  |  |  +--ElisionExpression ||\n"
      "|  |  +--BlockStatement |{}|\n",
      RunOn("/** @param {number} level @param {...string} args */\n"
            "function foo(level, ...args) {}"));
}

TEST_F(TypeResolverTest, TupleType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--TupleType\n"
      "|  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  +--Name |number|\n"
      "|  |  |  |  +--TypeName {%string%}\n"
      "|  |  |  |  |  +--Name |string|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {[%number%,%string%]}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {[number,string]} */ var foo"));
}

TEST_F(TypeResolverTest, TypeAlias) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@typedef|\n"
      "|  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  +--Name |number|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] {%number%}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression ||\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--TypeName {Foo@1001}\n"
      "|  |  |  |  +--Name |Foo|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[bar@1002] {Foo@1001}\n"
      "|  |  |  +--Name |bar|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @typedef {number} */ var Foo;\n"
            "/** @type {Foo} */ var bar;\n"));
}

TEST_F(TypeResolverTest, TypeGroup) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--NullableType\n"
      "|  |  |  |  +--TypeGroup\n"
      "|  |  |  |  |  +--UnionType\n"
      "|  |  |  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  |  |  +--Name |number|\n"
      "|  |  |  |  |  |  +--TypeName {%string%}\n"
      "|  |  |  |  |  |  |  +--Name |string|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] "
      "{%null%|%number%|%string%}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {?(number|string)} */ var foo;"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--NonNullableType\n"
      "|  |  |  |  +--TypeGroup\n"
      "|  |  |  |  |  +--UnionType\n"
      "|  |  |  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  |  |  +--Name |number|\n"
      "|  |  |  |  |  |  +--TypeName {%string%}\n"
      "|  |  |  |  |  |  |  +--Name |string|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {%number%|%string%}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n"
      "ANALYZER_ERROR_JSDOC_EXPECT_NULLABLE_TYPE@12:27\n",
      RunOn("/** @type {!(number|string)} */ var foo;"));
}

TEST_F(TypeResolverTest, TypeApplication) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@interface|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@template|\n"
      "|  |  |  +--TypeName {T@1001}\n"
      "|  |  |  |  +--Name |T|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Foo@1001] {Foo<T>@1003}\n"
      "|  |  |  +--Name |Foo|\n"
      "|  |  |  +--ElisionExpression || GenericInterface<T>[Foo@1002]\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@constructor|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@implements|\n"
      "|  |  |  +--TypeApplication\n"
      "|  |  |  |  +--TypeName {Foo<T>@1003}\n"
      "|  |  |  |  |  +--Name |Foo|\n"
      "|  |  |  |  +--Tuple\n"
      "|  |  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  |  +--Name |number|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[Bar@1005] "
      "{function(new:Bar@1004):Bar@1004}\n"
      "|  |  |  +--Name |Bar|\n"
      "|  |  |  +--ElisionExpression || Class[Bar@1006]\n"
      "Class[Bar@1006]\n"
      "+--BaseClasses\n"
      "|   +--Class[Object@504]\n"
      "|   +--ConstructedInterface<%number%>[Foo@1009]\n",
      RunOn("/** @interface @template T */ var Foo;"
            "/** @constructor @implements {Foo<number>} */ var Bar;"));
}

TEST_F(TypeResolverTest, UnionType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--UnionType\n"
      "|  |  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  |  +--Name |number|\n"
      "|  |  |  |  +--TypeName {%string%}\n"
      "|  |  |  |  |  +--Name |string|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {%number%|%string%}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {number|string} */ var foo"));
}

TEST_F(TypeResolverTest, UnknownType) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--UnknownType |?|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {*}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {?} */ var foo;"));
}

TEST_F(TypeResolverTest, Var) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@type|\n"
      "|  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  +--Name |number|\n"
      "|  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {%number%}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression ||\n",
      RunOn("/** @type {number} */ var foo;"));
}

TEST_F(TypeResolverTest, VarFunction) {
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@param|\n"
      "|  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  +--Name |number|\n"
      "|  |  |  +--ReferenceExpression ParameterVar[x@1002]\n"
      "|  |  |  |  +--Name |x|\n"
      "|  |  |  +--JsDocText |*/|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {function(%number%)}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression || Function[foo@1003]\n",
      RunOn("/** @param {number} x */ var foo;"));
  EXPECT_EQ(
      "Module\n"
      "+--Annotation\n"
      "|  +--JsDocDocument\n"
      "|  |  +--JsDocText |/**|\n"
      "|  |  +--JsDocTag\n"
      "|  |  |  +--Name |@return|\n"
      "|  |  |  +--TypeName {%number%}\n"
      "|  |  |  |  +--Name |number|\n"
      "|  |  |  +--JsDocText |x */|\n"
      "|  +--VarStatement\n"
      "|  |  +--BindingNameElement VarVar[foo@1001] {function():%number%}\n"
      "|  |  |  +--Name |foo|\n"
      "|  |  |  +--ElisionExpression || Function[foo@1002]\n",
      RunOn("/** @return {number} x */ var foo;"));
}

}  // namespace analyzer
}  // namespace aoba
