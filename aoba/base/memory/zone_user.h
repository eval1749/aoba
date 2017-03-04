// Copyright 2015 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_MEMORY_ZONE_USER_H_
#define AOBA_BASE_MEMORY_ZONE_USER_H_

#include "base/macros.h"
#include "aoba/base/base_export.h"

namespace aoba {

class Zone;

//////////////////////////////////////////////////////////////////////
//
// ZoneUser
//
class AOBA_BASE_EXPORT ZoneUser {
 public:
  ~ZoneUser();

  Zone* zone() const { return zone_; }

 protected:
  explicit ZoneUser(Zone* zone);

 private:
  Zone* const zone_;

  DISALLOW_COPY_AND_ASSIGN(ZoneUser);
};

}  // namespace aoba

#endif  // AOBA_BASE_MEMORY_ZONE_USER_H_
