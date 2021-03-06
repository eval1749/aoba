// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_BASE_CASTABLE_H_
#define AOBA_BASE_CASTABLE_H_

#include <type_traits>

#include "base/logging.h"

namespace aoba {

// T* as()
// const char* class_name()
// bool is<T>()
// static const char* static_class_name()
template <typename Base>
class Castable {
 public:
  template <class Class>
  Class& As() {
    auto* const result = TryAs<Class>();
    DCHECK(result) << " Expect " << Class::static_class_name() << " instead of "
                   << class_name() << ": " << static_cast<Base&>(*this);
    return *result;
  }

  template <class Class>
  const Class& As() const {
    const auto* const result = TryAs<Class>();
    DCHECK(result) << " Expect " << Class::static_class_name() << " instead of "
                   << class_name() << ": " << static_cast<const Base&>(*this);
    return *result;
  }

  virtual const char* class_name() const { return static_class_name(); }

  template <class Class>
  bool Is() const {
    static_assert(std::is_base_of<Base, Class>::value, "Unrelated classes");
    return IsClassOf(Class::static_class_name());
  }

  template <class Class>
  Class* TryAs() {
    return Is<Class>() ? static_cast<Class*>(this) : nullptr;
  }

  template <class Class>
  const Class* TryAs() const {
    return Is<Class>() ? static_cast<const Class*>(this) : nullptr;
  }

 protected:
  Castable() = default;
  virtual ~Castable() = default;

  virtual bool IsClassOf(const char* other_name) const {
    return static_class_name() == other_name;
  }

 public:
  static const char* static_class_name() { return "Castable"; }
};

#define DECLARE_CASTABLE_CLASS(this_name, base_name)                      \
 public:                                                                  \
  static const char* static_class_name() { return #this_name; }           \
                                                                          \
  const char* class_name() const override { return static_class_name(); } \
                                                                          \
 protected:                                                               \
  bool IsClassOf(const char* other_name) const override {                 \
    return static_class_name() == other_name ||                           \
           base_name::IsClassOf(other_name);                              \
  }

// For clang
#if 0
#define DECLARE_CASTABLE_CLASS(this_name, base_name)            \
 public:                                                        \
  static const char* static_class_name() { return #this_name; } \
                                                                \
  const char* class_name() const override;                      \
                                                                \
 protected:                                                     \
  bool IsClassOf(const char* other_name) const override;

#define DEFINE_CASTABLE_CLASS(this_name, base_name)                         \
  const char* this_name::class_name() const { return static_class_name(); } \
                                                                            \
  bool this_name::IsClassOf(const char* other_name) const {                 \
    return static_class_name() == other_name ||                             \
           base_name::IsClassOf(other_name);                                \
  }
#endif

}  // namespace aoba

#endif  // AOBA_BASE_CASTABLE_H_
