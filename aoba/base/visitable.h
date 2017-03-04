// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_VISITABLE_H_
#define AOBA_BASE_VISITABLE_H_

namespace aoba {

//
// Visitor pattern helper classes
//

template <class Visitor>
class ConstVisitable {
 public:
  virtual void Accept(Visitor* visitor) const = 0;

 protected:
  ConstVisitable() = default;
  ~ConstVisitable() = default;
};

template <class Visitor>
class Visitable {
 public:
  virtual void Accept(Visitor* visitor) const = 0;

 protected:
  Visitable() = default;
  ~Visitable() = default;
};

}  // namespace aoba

#endif  // AOBA_BASE_VISITABLE_H_
