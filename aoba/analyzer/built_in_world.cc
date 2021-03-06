// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unordered_map>

#include "aoba/analyzer/built_in_world.h"

#include "base/memory/singleton.h"
#include "base/strings/string_piece.h"
#include "base/strings/string_split.h"
#include "base/strings/utf_string_conversions.h"
#include "aoba/ast/compilation_units.h"
#include "aoba/ast/node_factory.h"
#include "aoba/ast/tokens.h"
#include "aoba/base/memory/zone.h"
#include "aoba/base/source_code.h"
#include "aoba/base/source_code_factory.h"
#include "aoba/base/source_code_range.h"

namespace aoba {
namespace analyzer {

namespace {

const char* const kSourceCode =
    "Array\n"
    "boolean\n"
    "global\n"
    "null\n"
    "number\n"
    "Object\n"
    "prototype\n"
    "string\n"
    "symbol\n"
    "undefined\n"
    "void\n";

const SourceCode& NewSourceCodeForBuildIn(Zone* zone) {
  SourceCode::Factory source_code_factory(zone);
  const auto& text16 = base::UTF8ToUTF16(kSourceCode);
  const auto size = text16.size() * sizeof(base::char16);
  auto* data = static_cast<base::char16*>(zone->Allocate(size));
  ::memcpy(data, text16.data(), size);
  return source_code_factory.New(base::FilePath(),
                                 base::StringPiece16(data, text16.size()));
}

}  // namespace

//
// BuiltInWorld::Private
//
class BuiltInWorld::Private final {
 public:
  Private();
  ~Private() = default;

  ast::NodeFactory& node_factory() { return node_factory_; }
  const std::vector<ast::TokenKind>& primitive_types() const;
  const SourceCode& source_code() const { return source_code_; }

  const ast::Node& NameOf(ast::TokenKind kind) const;
  const ast::Node& TypeOf(ast::TokenKind kind) const;

 private:
  void PopulateNameTable();
  void RegisterTypes();

  Zone zone_;

  std::unordered_map<ast::TokenKind, const ast::Node*> name_map_;

  // |ast::NodeFactory| constructor takes |zone_|.
  ast::NodeFactory node_factory_;
  const std::vector<ast::TokenKind> primitive_types_;
  const SourceCode& source_code_;

  // Mapping from known type name to type AST.
  std::unordered_map<ast::TokenKind, const ast::Node*> type_map_;

  DISALLOW_COPY_AND_ASSIGN(Private);
};

BuiltInWorld::Private::Private()
    : zone_("BuiltInWorld"),
      node_factory_(&zone_),
      primitive_types_({ast::TokenKind::Boolean, ast::TokenKind::Null,
                        ast::TokenKind::Number, ast::TokenKind::String,
                        ast::TokenKind::Symbol, ast::TokenKind::Undefined,
                        ast::TokenKind::Void}),
      source_code_(NewSourceCodeForBuildIn(&zone_)) {
  PopulateNameTable();
  RegisterTypes();
}

const std::vector<ast::TokenKind>& BuiltInWorld::Private::primitive_types()
    const {
  return primitive_types_;
}

const ast::Node& BuiltInWorld::Private::NameOf(ast::TokenKind kind) const {
  const auto& it = name_map_.find(kind);
  DCHECK(it != name_map_.end()) << static_cast<int>(kind);
  return *it->second;
}

void BuiltInWorld::Private::PopulateNameTable() {
  const auto& names = base::SplitStringPiece(
      kSourceCode, "\n", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
  for (const auto name : names) {
    const auto name_start = static_cast<int>(name.data() - kSourceCode);
    const auto name_end = static_cast<int>(name_start + name.size());
    const auto& node =
        node_factory_.NewName(source_code_.Slice(name_start, name_end));
    const auto& result = name_map_.emplace(ast::Name::KindOf(node), &node);
    DCHECK(result.second) << "Multiple occurrence of " << *result.first->second
                          << ' ' << node;
  }
}

const ast::Node& BuiltInWorld::Private::TypeOf(ast::TokenKind kind) const {
  const auto& it = type_map_.find(kind);
  DCHECK(it != type_map_.end()) << static_cast<int>(kind);
  return *it->second;
}

void BuiltInWorld::Private::RegisterTypes() {
  for (const auto id : primitive_types_) {
    const auto& name = NameOf(id);
    const auto& node = node_factory_.NewPrimitiveType(name);
    const auto& result = type_map_.emplace(id, &node);
    DCHECK(result.second) << "Multiple occurrence of " << name;
  }
}

//
// BuiltInWorld
//
BuiltInWorld::BuiltInWorld()
    : private_(std::make_unique<Private>()),
      global_module_(private_->node_factory().NewModule(
          SourceCodeRange::CollapseToStart(private_->source_code().range()),
          {})) {}

BuiltInWorld::~BuiltInWorld() = default;

const std::vector<ast::TokenKind>& BuiltInWorld::primitive_types() const {
  return private_->primitive_types();
}

const ast::Node& BuiltInWorld::NameOf(ast::TokenKind kind) const {
  return private_->NameOf(kind);
}

const ast::Node& BuiltInWorld::TypeOf(ast::TokenKind kind) const {
  return private_->TypeOf(kind);
}

// static
BuiltInWorld* BuiltInWorld::GetInstance() {
  return base::Singleton<BuiltInWorld>::get();
}

}  // namespace analyzer
}  // namespace aoba
