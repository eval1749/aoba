// Copyright (c) 2016 Project Vogue. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef AOBA_TESTING_SIMPLE_FORMATTER_H_
#define AOBA_TESTING_SIMPLE_FORMATTER_H_

#include <iosfwd>

namespace aoba {
namespace ast {
class Node;
}
namespace parser {

struct Formatted {
  const ast::Node* node;
};

Formatted AsFormatted(const ast::Node& node);

std::ostream& operator<<(std::ostream& ostream, const Formatted& formatted);

}  // namespace parser
}  // namespace aoba

#endif  // AOBA_TESTING_SIMPLE_FORMATTER_H_
