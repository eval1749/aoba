// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_PARSER_PUBLIC_PARSER_CONTEXT_BUILDER_H_
#define AOBA_PARSER_PUBLIC_PARSER_CONTEXT_BUILDER_H_

#include <memory>

#include "aoba/parser/public/parser_context.h"

namespace aoba {

namespace ast {
class NodeFactory;
}

//
// ParserContext::Builder
//
class AOBA_PARSER_EXPORT ParserContext::Builder {
 public:
  Builder();
  ~Builder();

  std::unique_ptr<ParserContext> Build() const;

  Builder& set_error_sink(ErrorSink* error_sink);
  Builder& set_node_factory(ast::NodeFactory* node_factory);

 private:
  friend class ParserContext;

  ErrorSink* error_sink_ = nullptr;
  ast::NodeFactory* node_factory_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(Builder);
};

}  // namespace aoba

#endif  // AOBA_PARSER_PUBLIC_PARSER_CONTEXT_BUILDER_H_
