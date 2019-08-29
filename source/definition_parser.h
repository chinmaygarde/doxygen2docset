
#pragma once

#include <tinyxml2.h>
#include <string>
#include <vector>

#include "macros.h"
#include "token.h"

namespace d2d {

class DefinitionParser {
 public:
  DefinitionParser(const std::string& file_path);

  ~DefinitionParser();

  bool IsValid() const;

  std::vector<Token> ReadTokens() const;

 private:
  tinyxml2::XMLDocument xml_document_;
  bool is_valid_ = false;

  D2D_DISALLOW_COPY_AND_ASSIGN(DefinitionParser);
};

}  // namespace d2d
