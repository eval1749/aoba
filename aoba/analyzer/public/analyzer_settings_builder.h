// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_PUBLIC_ANALYZER_SETTINGS_BUILDER_H_
#define AOBA_ANALYZER_PUBLIC_ANALYZER_SETTINGS_BUILDER_H_

#include <memory>

#include "aoba/analyzer/public/analyzer_settings.h"

namespace aoba {

//
// AnalyzerSettings::Builder
//
class AOBA_ANALYZER_EXPORT AnalyzerSettings::Builder final {
 public:
  Builder();
  ~Builder();

  Builder& set_error_sink(ErrorSink* error_sink);
  Builder& set_zone(Zone* zone);

  std::unique_ptr<AnalyzerSettings> Build();

 private:
  friend class AnalyzerSettings;

  ErrorSink* error_sink_ = nullptr;
  Zone* zone_ = nullptr;

  DISALLOW_COPY_AND_ASSIGN(Builder);
};

}  // namespace aoba

#endif  // AOBA_ANALYZER_PUBLIC_ANALYZER_SETTINGS_BUILDER_H_
