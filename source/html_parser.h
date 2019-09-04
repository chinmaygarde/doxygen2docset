
#pragma once

#include <gumbo.h>

#include <string>
#include <vector>

#include "file.h"
#include "macros.h"
#include "token.h"

namespace d2d {

class HTMLParser {
 public:
  HTMLParser(std::unique_ptr<AutoMapping> mapping);

  ~HTMLParser();

  bool IsValid() const;

  Allocation BuildHTMLWithTOC(const std::vector<Token>& tokens) const;

 private:
  std::unique_ptr<AutoMapping> mapping_;
  GumboOutput* parser_ = nullptr;
  bool is_valid_ = false;

  D2D_DISALLOW_COPY_AND_ASSIGN(HTMLParser);
};

}  // namespace d2d
