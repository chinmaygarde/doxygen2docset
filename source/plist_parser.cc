#include "plist_parser.h"

#include <sstream>

#include "file.h"
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

std::string PlistParser::ReadStringForKey(const std::string& key) const {
  if (!is_valid_) {
    D2D_ERROR << "XML Plist could not be parsed.";
    return "";
  }
  if (auto plist = xml_document_.FirstChildElement("plist")) {
    if (auto dict = plist->FirstChildElement("dict")) {
      for (auto child = dict->FirstChildElement(); child != nullptr;
           child = child->NextSiblingElement()) {
        if (strncmp(child->GetText(), key.data(), key.size()) == 0) {
          if (auto version = child->NextSiblingElement("string")) {
            return version->GetText();
          }
        }
      }
    }
  }
  return "";
}

std::string PlistParser::ReadDocsetID() const {
  return ReadStringForKey("CFBundleIdentifier");
}

std::string PlistParser::ReadDocsetName() const {
  return ReadStringForKey("CFBundleName");
}

bool WriteDocSetPlist(const std::string& bundle_identifier,
                      const std::string& bundle_name, const std::string& path) {
  std::stringstream stream;

  stream << R"~~~(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
     <key>CFBundleIdentifier</key>
     <string>)~~~"
         << bundle_identifier << R"~~~(</string>
     <key>CFBundleName</key>
     <string>)~~~"
         << bundle_name << R"~~~(</string>
     <key>DocSetPlatformFamily</key>
     <string>)~~~"
         << bundle_identifier << R"~~~(</string>
     <key>isDashDocset</key>
     <true/>
     <key>DashDocSetFamily</key>
     <string>dashtoc</string>
</dict>
</plist>)~~~";

  auto string = stream.str();
  return CopyData(string.data(), string.size(), path);
}

}  // namespace d2d
