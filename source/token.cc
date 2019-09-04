#include "token.h"

#include <map>
#include <string>

#include "logger.h"

namespace d2d {

Token::Token(const tinyxml2::XMLElement* element) {
  if (element == nullptr) {
    return;
  }

  if (auto token_identifier_xml =
          element->FirstChildElement("TokenIdentifier")) {
    if (auto xml = token_identifier_xml->FirstChildElement("Name")) {
      name_ = xml->GetText();
    }
    if (auto xml = token_identifier_xml->FirstChildElement("APILanguage")) {
      language_ = xml->GetText();
    }
    if (auto xml = token_identifier_xml->FirstChildElement("Type")) {
      type_ = xml->GetText();
    }
    if (auto xml = token_identifier_xml->FirstChildElement("Scope")) {
      scope_ = xml->GetText();
    }
  }

  if (auto xml = element->FirstChildElement("Path")) {
    path_ = xml->GetText();
  }

  if (auto xml = element->FirstChildElement("Anchor")) {
    anchor_ = xml->GetText();
  }

  if (auto xml = element->FirstChildElement("DeclaredIn")) {
    declared_in_ = xml->GetText();
  }

  is_valid_ = true;
}

Token::Token() = default;

Token::Token(Token&&) = default;

Token::Token(const Token& token) = default;

Token::~Token() = default;

Token& Token::operator=(const Token&) = default;

bool Token::IsValid() const { return is_valid_; }

const std::string& Token::GetName() const { return name_; }

const std::string& Token::GetLanguage() const { return language_; }

const std::string& Token::GetType() const { return type_; }

const std::string& Token::GetScope() const { return scope_; }

const std::string& Token::GetPath() const { return path_; }

const std::string& Token::GetAnchor() const { return anchor_; }

const std::string& Token::GetDeclaredIn() const { return declared_in_; }

std::string Token::GetIndexName() const {
  // Get rid of the namespace in the name.
  {
    auto found = name_.find_last_of("::");
    if (found != std::string::npos) {
      return name_.substr(found + 1);
    }
  }

  return name_;
}

static const std::map<std::string, std::string> kDocsetTypeMap = {
    {"cat", "Category"},    //
    {"cl", "Class"},        //
    {"clm", "Method"},      //
    {"data", "Variable"},   //
    {"econst", "Enum"},     //
    {"ffunc", "Class"},     //
    {"func", "Function"},   //
    {"instm", "Method"},    //
    {"instp", "Variable"},  //
    {"intf", "Class"},      //
    {"intfcm", "Method"},   //
    {"intfm", "Method"},    //
    {"intfp", "Variable"},  //
    {"macro", "Macro"},     //
    {"ns", "Namespace"},    //
    {"tdef", "Type"},       //
    {"tmplt", "Class"},     //
};

std::string Token::GetIndexType() const {
  auto found = kDocsetTypeMap.find(type_);
  if (found != kDocsetTypeMap.end()) {
    return found->second;
  }

  return "Data";
}

std::string Token::GetIndexPath() const { return path_ + "#" + anchor_; }

Token::TOCLinks Token::GetTokensByFile(const std::vector<Token>& tokens) {
  TOCLinks links;
  for (const auto& token : tokens) {
    links[token.path_].push_back(token);
  }
  return links;
}

}  // namespace d2d
