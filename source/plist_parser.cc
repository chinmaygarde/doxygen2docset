#include "plist_parser.h"

#include "logger.h"

namespace d2d {

PlistParser::PlistParser(const std::string& plist_path) {
  if (xml_document_.LoadFile(plist_path.c_str()) != tinyxml2::XML_SUCCESS) {
    D2D_ERROR << "Could not parse XML file: " << plist_path;
    return;
  }

  is_valid_ = true;
}

PlistParser::~PlistParser() = default;

bool PlistParser::IsValid() const { return is_valid_; }

std::string PlistParser::ReadDocsetName() const {
  if (!is_valid_) {
    D2D_ERROR << "XML Plist could not be parsed.";
    return "";
  }
  if (auto plist = xml_document_.FirstChildElement("plist")) {
    if (auto dict = plist->FirstChildElement("dict")) {
      for (auto child = dict->FirstChildElement(); child != nullptr;
           child = child->NextSiblingElement()) {
        if (strncmp(child->GetText(), "CFBundleIdentifier",
                    strlen("CFBundleIdentifier")) == 0) {
          if (auto version = child->NextSiblingElement("string")) {
            return version->GetText();
          }
        }
      }
    }
  }
  return "";
}

}  // namespace d2d
