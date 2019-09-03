
#pragma once

#include <gumbo.h>

#include <string>

#include "macros.h"

namespace d2d {

class HTMLParser {
 public:
  HTMLParser(const std::string& path);

  ~HTMLParser();

  bool IsValid() const;

 private:
  GumboOutput* parser_ = nullptr;
  bool is_valid_ = false;

  D2D_DISALLOW_COPY_AND_ASSIGN(HTMLParser);
};

}  // namespace d2d
