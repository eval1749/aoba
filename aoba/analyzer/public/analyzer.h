// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_PUBLIC_ANALYZER_H_
#define AOBA_ANALYZER_PUBLIC_ANALYZER_H_

#include <memory>

#include "base/macros.h"
#include "aoba/analyzer/public/analyzer_export.h"

namespace aoba {
namespace ast {
class Node;
}

namespace analyzer {
class Controller;
}  // namespace analyzer

class AnalyzerSettings;

//
// Analyzer provides public API of analyzer.
//
class AOBA_ANALYZER_EXPORT Analyzer final {
 public:
  explicit Analyzer(const AnalyzerSettings& settings);
  ~Analyzer();

  // Returns built-in module for error message.
  const ast::Node& built_in_module() const;

  void Analyze();
  void Load(const ast::Node& node);

 private:
  std::unique_ptr<analyzer::Controller> controller_;

  DISALLOW_COPY_AND_ASSIGN(Analyzer);
};

}  // namespace aoba

#endif  // AOBA_ANALYZER_PUBLIC_ANALYZER_H_
