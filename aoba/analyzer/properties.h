// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_ANALYZER_PROPERTIES_H_
#define AOBA_ANALYZER_PROPERTIES_H_

#include "base/macros.h"
#include "base/strings/string_piece.h"
#include "aoba/base/memory/zone_allocated.h"
#include "aoba/base/memory/zone_unordered_map.h"
#include "aoba/base/memory/zone_vector.h"

namespace aoba {

namespace ast {
class Node;
}

namespace analyzer {

class Property;

//
// Properties
//
class Properties final : public ZoneAllocated {
 public:
  class Editor;

  ~Properties();

  const Property& Get(const ast::Node& key) const;

  // Returns associated property of |key| or null if there is no associated
  // property.
  const Property* TryGet(const ast::Node& key) const;

 private:
  friend class Factory;

  Properties(Zone* zone, const ast::Node& node);

  // TODO(eval1749): We should use another way to handle computed property
  // name.
  ZoneUnorderedMap<base::StringPiece16,
                   const Property*,
                   base::StringPiece16Hash>
      computed_name_map_;

  ZoneUnorderedMap<int, const Property*> name_map_;

  // AST node which creates this |Properties|.
  const ast::Node& owner_;

  DISALLOW_COPY_AND_ASSIGN(Properties);
};

}  // namespace analyzer
}  // namespace aoba

#endif  // AOBA_ANALYZER_PROPERTIES_H_
