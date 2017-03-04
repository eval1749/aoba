// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/testing/lexer_test_base.h"

#include "base/strings/utf_string_conversions.h"
#include "aoba/base/source_code_factory.h"
#include "aoba/parser/public/parser_context.h"
#include "aoba/parser/public/parser_context_builder.h"
#include "aoba/parser/utils/character_reader.h"
#include "aoba/testing/simple_error_sink.h"

namespace aoba {

LexerTestBase::LexerTestBase()
    : zone_("LexerTestBase"),
      node_factory_(&zone_),
      context_(ParserContext::Builder()
                   .set_error_sink(&error_sink_)
                   .set_node_factory(&node_factory_)
                   .Build()),
      source_code_factory_(&zone_) {}

void LexerTestBase::PrepareSouceCode(base::StringPiece script_text) {
  const auto& script_text16 = base::UTF8ToUTF16(script_text);
  source_code_ = &source_code_factory_.New(base::FilePath(),
                                           base::StringPiece16(script_text16));
  error_sink_.Reset();
}

LexerTestBase::~LexerTestBase() = default;

const SourceCode& LexerTestBase::source_code() const {
  DCHECK(source_code_);
  return *source_code_;
}

}  // namespace aoba
