// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_CONTROLLER_H_
#define AOBA_ANALYZER_CONTROLLER_H_

#include <memory>
#include <vector>

#include "base/macros.h"

namespace aoba {

namespace ast {
class Node;
}

class AnalyzerSettings;

namespace analyzer {

class Context;
class Factory;

//
// Controller
//
class Controller final {
 public:
  explicit Controller(const AnalyzerSettings& settings);
  ~Controller();

  const ast::Node& built_in_module() const;

  void Analyze();
  void Load(const ast::Node& node);

 private:
  Factory& factory() const;

  void DumpValues();
  void PrintTree();

  const std::unique_ptr<Context> context_;
  std::vector<const ast::Node*> nodes_;

  DISALLOW_COPY_AND_ASSIGN(Controller);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_CONTROLLER_H_
