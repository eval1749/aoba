// Copyright 2014-2015 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "aoba/base/memory/zone_allocated.h"

#include "base/logging.h"
#include "aoba/base/memory/zone.h"

namespace aoba {

ZoneAllocated::ZoneAllocated() {}

#if defined(COMPILER_MSVC) && !defined(__clang__)
ZoneAllocated::~ZoneAllocated() {
  NOTREACHED();
}
#endif

#if 0
void ZoneAllocated::operator delete(void* pointer, Zone* zone) {
  __assume(pointer);
  __assume(zone);
  NOTREACHED();
}
#endif

void* ZoneAllocated::operator new(size_t size, Zone* zone) {
  return zone->Allocate(size);
}

}  // namespace aoba
