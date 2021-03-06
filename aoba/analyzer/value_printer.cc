// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iterator>
#include <ostream>
#include <type_traits>

#include "base/logging.h"
#include "aoba/analyzer/types.h"
#include "aoba/analyzer/value_forward.h"
#include "aoba/analyzer/values.h"
#include "aoba/ast/declarations.h"
#include "aoba/ast/node_printer.h"
#include "aoba/ast/tokens.h"

namespace aoba {
namespace analyzer {

namespace {

const ast::Node* ValueNameOf(const Value& value) {
  if (value.Is<Function>())
    return &value.As<Function>().name();
  const auto& node = value.node();
  if (value.Is<Class>())
    return &value.As<Class>().name();
  if (value.Is<Property>())
    return &node;
  if (value.Is<Variable>())
    return &node;
  return nullptr;
}

//
// NamedValue
//
struct NamedValue {
  const Value* value;
};

std::ostream& operator<<(std::ostream& ostream, const NamedValue& named) {
  const auto& value = *named.value;
  const auto* name = ValueNameOf(value);
  if (!name || !CanBeValueName(*name))
    return ostream << '@' << value.id();
  return ostream << '[' << ast::AsSourceCode(*name) << '@' << value.id() << ']';
}

//
// Printable
//
template <typename T>
struct Printable {
  static_assert(std::is_base_of<Value, T>::value, "Should be Value");
  const T* value;
};

Printable<Value> AsPrintable(const Value& value) {
  return Printable<Value>{&value};
}

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<ConstructedClass>& printable) {
  const auto& value = *printable.value;
  ostream << "Constructed" << value.kind();
  const auto* delimiter = "<";
  for (const auto& argument : value.arguments()) {
    ostream << delimiter << argument;
    delimiter = ",";
  }
  if (*delimiter == ',')
    ostream << '>';
  return ostream << NamedValue{&value};
}

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<Function>& printable) {
  const auto& value = *printable.value;
  return ostream << "Function" << NamedValue{&value};
}

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<GenericClass>& printable) {
  const auto& value = *printable.value;
  ostream << "Generic" << value.kind();
  const auto* delimiter = "<";
  for (const auto& parameter : value.parameters()) {
    ostream << delimiter << ast::AsSourceCode(parameter.name());
    delimiter = ",";
  }
  if (*delimiter == ',')
    ostream << '>';
  return ostream << NamedValue{&value};
}

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<NormalClass>& printable) {
  const auto& value = *printable.value;
  return ostream << value.kind() << NamedValue{&value};
}

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<OrdinaryObject>& printable) {
  const auto& value = *printable.value;
  return ostream << value.class_name() << '@' << value.id();
}

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<Property>& printable) {
  const auto& value = *printable.value;
  return ostream << value.visibility() << value.class_name() << '['
                 << ast::AsSourceCode(value.key()) << '@' << value.id() << ']';
}

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<Undefined>& printable) {
  const auto& value = *printable.value;
  return ostream << "Undefined@" << value.id() << ' ' << value.node();
}

std::ostream& operator<<(std::ostream& ostream,
                         const Printable<Variable>& printable) {
  const auto& value = *printable.value;
  return ostream << value.kind() << "Var["
                 << ast::AsSourceCode(*ValueNameOf(value)) << '@' << value.id()
                 << ']';
}

// Dispatcher
std::ostream& operator<<(std::ostream& ostream,
                         const Printable<Value>& printable) {
  const auto& value = *printable.value;
#define V(name)                            \
  if (auto* derived = value.TryAs<name>()) \
    return ostream << Printable<name>{derived};
  FOR_EACH_ANALYZE_VALUE(V)
#undef V

  NOTREACHED() << "Need operator<< for " << value.class_name();
  return ostream;
}

}  // namespace

std::ostream& operator<<(std::ostream& ostream, ClassKind kind) {
  switch (kind) {
    case ClassKind::Class:
      return ostream << "Class";
    case ClassKind::Interface:
      return ostream << "Interface";
    case ClassKind::Record:
      return ostream << "Record";
  }
  return ostream << "ClassKind" << static_cast<int>(kind);
}

std::ostream& operator<<(std::ostream& ostream, const Value& value) {
  return ostream << AsPrintable(value);
}

std::ostream& operator<<(std::ostream& ostream, const Value* value) {
  if (!value)
    return ostream << "(null)";
  return ostream << *value;
}

std::ostream& operator<<(std::ostream& ostream, VariableKind kind) {
  static const char* kTexts[] = {
#define V(name) #name,
      FOR_EACH_VARIABLE_KIND(V)
#undef V
  };
  auto** it = std::begin(kTexts) + static_cast<size_t>(kind);
  if (it < std::begin(kTexts) || it >= std::end(kTexts))
    return ostream << "VariableKind" << static_cast<size_t>(kind);
  return ostream << *it;
}

std::ostream& operator<<(std::ostream& ostream, Visibility kind) {
  static const char* kTexts[] = {
#define V(name) #name,
      FOR_EACH_VISIBILITY(V)
#undef V
  };
  auto** it = std::begin(kTexts) + static_cast<size_t>(kind);
  if (it < std::begin(kTexts) || it >= std::end(kTexts))
    return ostream << "Visibility" << static_cast<size_t>(kind);
  return ostream << *it;
}

}  // namespace analyzer
}  // namespace aoba
