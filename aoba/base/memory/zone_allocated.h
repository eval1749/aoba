// Copyright 2014-2015 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_MEMORY_ZONE_ALLOCATED_H_
#define AOBA_BASE_MEMORY_ZONE_ALLOCATED_H_

#include "base/macros.h"
#include "aoba/base/base_export.h"

namespace aoba {

class Zone;

//////////////////////////////////////////////////////////////////////
//
// ZoneAllocated
//
class AOBA_BASE_EXPORT ZoneAllocated {
 public:
  void* operator new(size_t size, Zone* zone);

  // |ZoneAllocated| can't have |delete| operator. But MSVC requires them.
  void operator delete(void*, Zone*) = delete;

 protected:
  ZoneAllocated();

  // |ZoneAllocated| can't have destructor operator. But MSVC requires them.
  ~ZoneAllocated();
};

}  // namespace aoba

#endif  // AOBA_BASE_MEMORY_ZONE_ALLOCATED_H_
