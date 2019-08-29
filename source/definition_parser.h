
#pragma once

#include <string>

#include "macros.h"

namespace d2d {

class DefinitionParser {
 public:
  DefinitionParser(const std::string& file_path);

  ~DefinitionParser();

  bool IsValid() const;

 private:
  bool is_valid_ = false;

  D2D_DISALLOW_COPY_AND_ASSIGN(DefinitionParser);
};

}  // namespace d2d
