// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_PUBLIC_ANALYZER_SETTINGS_H_
#define AOBA_ANALYZER_PUBLIC_ANALYZER_SETTINGS_H_

#include <memory>

#include "base/macros.h"
#include "aoba/analyzer/public/analyzer_export.h"

namespace aoba {
class ErrorSink;
class Zone;

namespace ast {
class Node;
}

//
// AnalyzerSettings
//
class AOBA_ANALYZER_EXPORT AnalyzerSettings final {
 public:
  class AOBA_ANALYZER_EXPORT Builder;

  ~AnalyzerSettings();

  ErrorSink& error_sink() const;
  Zone& zone() const;

 private:
  explicit AnalyzerSettings(const Builder& builder);

  ErrorSink& error_sink_;
  Zone& zone_;

  DISALLOW_COPY_AND_ASSIGN(AnalyzerSettings);
};

}  // namespace aoba

#endif  // AOBA_ANALYZER_PUBLIC_ANALYZER_SETTINGS_H_
