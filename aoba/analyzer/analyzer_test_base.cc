// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/analyzer/analyzer_test_base.h"

#include "aoba/analyzer/context.h"
#include "aoba/analyzer/public/analyzer_settings_builder.h"
#include "aoba/parser/public/parse.h"
#include "aoba/testing/simple_error_sink.h"

namespace aoba {
namespace analyzer {

//
// AnalyzerTestBase
//
AnalyzerTestBase::AnalyzerTestBase()
    : settings_(AnalyzerSettings::Builder()
                    .set_error_sink(&error_sink())
                    .set_zone(&zone())
                    .Build()) {}

AnalyzerTestBase::~AnalyzerTestBase() = default;

std::unique_ptr<Context> AnalyzerTestBase::NewContext() {
  return std::make_unique<Context>(*settings_);
}

const ast::Node& AnalyzerTestBase::ParseAsModule(
    base::StringPiece script_text) {
  PrepareSouceCode(script_text);
  ParserOptions options;
  return Parse(&context(), source_code().range(), options);
}

}  // namespace analyzer
}  // namespace aoba
