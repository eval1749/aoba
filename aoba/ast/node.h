// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_AST_NODE_H_
#define AOBA_AST_NODE_H_

#include <iosfwd>

#include "base/macros.h"
#include "aoba/ast/ast_export.h"
#include "aoba/base/source_code_range.h"

namespace aoba {
namespace ast {

enum class TokenKind;
class Syntax;
enum class SyntaxCode;

//
// Node represents a node of Abstract Syntax Tree (AST).
//
class AOBA_AST_EXPORT Node final {
 public:
  ~Node();

  // |Node| has variable number of child nodes and allocated in |Zone|.
  void* operator new(size_t size, void* pointer) { return pointer; }

  bool operator==(const Node& other) const;
  bool operator==(const Node* other) const;
  bool operator!=(const Node& other) const;
  bool operator!=(const Node* other) const;

  // For ease of type checking and using DCHECK_EQ(), |Node| has |operator==()|.
  bool operator==(SyntaxCode syntax_code) const;
  bool operator!=(SyntaxCode syntax_code) const;
  bool operator==(TokenKind kind) const;
  bool operator!=(TokenKind kind) const;

  // Returns number of operands in this node.
  size_t arity() const { return arity_; }

  // Returns |index|th child of this node.
  const Node& child_at(size_t index) const;

  // Returns source code of this node.
  const SourceCode& source_code() const { return range_.source_code(); }

  // Returns source code range of this node.
  const SourceCodeRange& range() const { return range_; }

  // Returns |Syntax| of this node.
  const Syntax& syntax() const { return syntax_; }

  // Short cut for checking whether this |Node is literal or no.
  bool is_literal() const;

  bool Is(SyntaxCode syntax_code) const;
  bool Is(TokenKind kind) const;

  template <typename T>
  bool Is() const {
    static_assert(std::is_base_of<Syntax, T>::value, "Should be Syntax class");
    return Is(T::kSyntaxCode);
  }

 protected:
  Node(const SourceCodeRange& range, const Syntax& syntax, size_t arity);

 private:
  friend class NodeFactory;

  // TODO(eval1749): Once we need to have other |int| values, we should reduce
  // size of |arity_|.
  // |arity_| holds number of child nodes. This value equals to
  // |syntax_.arity()| if |!syntax_.is_variadic()|.
  const size_t arity_;

  // Range of source code where this node comes from.
  const SourceCodeRange range_;

  // The syntax of this node.
  const Syntax& syntax_;

  // |nodes_| should be the last member of this class. Child nodes are allocated
  // starting from here.
  const Node* nodes_[1];

  DISALLOW_COPY_AND_ASSIGN(Node);
};

AOBA_AST_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                          const Node& node);
AOBA_AST_EXPORT std::ostream& operator<<(std::ostream& ostream,
                                          const Node* node);

}  // namespace ast
}  // namespace aoba

#endif  // AOBA_AST_NODE_H_
