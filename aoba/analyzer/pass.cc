// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/analyzer/pass.h"

#include "aoba/analyzer/context.h"
#include "aoba/ast/node.h"
#include "aoba/base/error_sink.h"

namespace aoba {
namespace analyzer {

//
// Pass
//
Pass::Pass(Context* context) : ContextUser(context) {}
Pass::~Pass() = default;

void Pass::RunOnAll() {}

}  // namespace analyzer
}  // namespace aoba
