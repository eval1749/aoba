// Copyright 2014-2015 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_MEMORY_ZONE_ALLOCATED_H_
#define AOBA_BASE_MEMORY_ZONE_ALLOCATED_H_

#include "aoba/base/base_export.h"
#include "base/macros.h"
#include "build/build_config.h"

namespace aoba {

class Zone;

//////////////////////////////////////////////////////////////////////
//
// ZoneAllocated
//
class AOBA_BASE_EXPORT ZoneAllocated {
 public:
  void* operator new(size_t size, Zone* zone);

#if defined(COMPILER_MSVC) && !defined(__clang__)
  // |ZoneAllocated| can't have |delete| operator. But MSVC requires them.
  void operator delete(void*, Zone*) = delete;
#endif

 protected:
  ZoneAllocated();

#if defined(COMPILER_MSVC) && !defined(__clang__)
  // |ZoneAllocated| can't have destructor operator. But MSVC requires them.
  ~ZoneAllocated();
#endif
};

}  // namespace aoba

#endif  // AOBA_BASE_MEMORY_ZONE_ALLOCATED_H_
