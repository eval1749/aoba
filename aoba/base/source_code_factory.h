// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_SOURCE_CODE_FACTORY_H_
#define AOBA_BASE_SOURCE_CODE_FACTORY_H_

#include "aoba/base/source_code.h"

namespace aoba {

class Zone;

class AOBA_BASE_EXPORT SourceCode::Factory {
 public:
  explicit Factory(Zone* zone);
  ~Factory();

  const SourceCode& New(const base::FilePath& file_path,
                        base::StringPiece16 file_contents);

 private:
  Zone& zone_;

  DISALLOW_COPY_AND_ASSIGN(Factory);
};

}  // namespace aoba

#endif  // AOBA_BASE_SOURCE_CODE_FACTORY_H_
