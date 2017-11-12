// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_CHECKER_EXTERNS_MODULE_H_
#define AOBA_CHECKER_EXTERNS_MODULE_H_

#include <vector>

namespace aoba {

struct ExternsFile {
  const char* name;
  size_t content_size;
  const char* content;
};

struct ExternsModule {
  const char* name;
  std::vector<ExternsFile> files;

  ExternsModule(const char* name, const std::vector<ExternsFile>& files);
  ExternsModule(const ExternsModule&);
  ~ExternsModule();
};

}  // namespace aoba

#endif  // AOBA_CHECKER_EXTERNS_MODULE_H_
