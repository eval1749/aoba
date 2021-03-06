// Copyright 2014-2015 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_MEMORY_ZONE_H_
#define AOBA_BASE_MEMORY_ZONE_H_

#include "base/macros.h"
#include "aoba/base/base_export.h"

namespace aoba {

//////////////////////////////////////////////////////////////////////
//
// Zone
//
class AOBA_BASE_EXPORT Zone final {
 public:
  Zone(const Zone& other) = delete;
  Zone(Zone&& other);
  explicit Zone(const char* name);
  ~Zone();

  Zone& operator=(const Zone& other) = delete;
  Zone& operator=(Zone&& other);

  // Allocate |size| bytes of memory in the Zone.
  void* Allocate(size_t size);

  template <typename T>
  T* AllocateObjects(size_t length) {
    return static_cast<T*>(Allocate(length * sizeof(T)));
  }

 private:
  class Segment;

  const char* const name_;
  Segment* segment_;
};

}  // namespace aoba

#endif  // AOBA_BASE_MEMORY_ZONE_H_
