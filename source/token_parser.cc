
#include "token_parser.h"

#include "logger.h"

namespace d2d {

TokenParser::TokenParser(const std::string& file_path) {
  if (xml_document_.LoadFile(file_path.c_str()) != tinyxml2::XML_SUCCESS) {
    D2D_ERROR << "Could not parse XML file: " << file_path;
    return;
  }

  is_valid_ = true;
}

TokenParser::~TokenParser() = default;

bool TokenParser::IsValid() const { return is_valid_; }

std::vector<Token> TokenParser::ReadTokens() const {
  if (!is_valid_) {
    return {};
  }

  std::vector<Token> tokens;

  if (auto xml_tokens = xml_document_.FirstChildElement("Tokens")) {
    for (auto xml_token = xml_tokens->FirstChildElement("Token");
         xml_token != nullptr;
         xml_token = xml_token->NextSiblingElement("Token")) {
      Token token(xml_token);
      if (token.IsValid()) {
        tokens.emplace_back(std::move(token));
      }
    }
  }

  return tokens;
}

}  // namespace d2d
