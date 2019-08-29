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

  std::string ReadDocsetID() const;

 private:
  tinyxml2::XMLDocument xml_document_;
  bool is_valid_ = false;

  D2D_DISALLOW_COPY_AND_ASSIGN(PlistParser);
};

bool WriteDocSetPlist(const std::string& bundle_identifier,
                      const std::string& bundle_name, const std::string& path);

}  // namespace d2d
