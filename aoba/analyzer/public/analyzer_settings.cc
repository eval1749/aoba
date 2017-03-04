// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/analyzer/public/analyzer_settings.h"

#include "aoba/analyzer/public/analyzer_settings_builder.h"

namespace aoba {

//
// AnalyzerSettings
//
AnalyzerSettings::AnalyzerSettings(const Builder& builder)
    : error_sink_(*builder.error_sink_), zone_(*builder.zone_) {}

AnalyzerSettings::~AnalyzerSettings() = default;

ErrorSink& AnalyzerSettings::error_sink() const {
  return error_sink_;
}

Zone& AnalyzerSettings::zone() const {
  return zone_;
}

}  // namespace aoba
