#pragma once

#include <tinyxml2.h>
#include <string>

#include "macros.h"

namespace d2d {

class PlistParser {
 public:
  PlistParser(const std::string& plist_path);

  ~PlistParser();

  bool IsValid() const;

  std::string ReadDocsetName() const;

 private:
  tinyxml2::XMLDocument xml_document_;
  bool is_valid_ = false;

  D2D_DISALLOW_COPY_AND_ASSIGN(PlistParser);
};

}  // namespace d2d
