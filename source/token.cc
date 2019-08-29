#include "token.h"

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

Token::Token(Token&&) = default;

Token::~Token() = default;

bool Token::IsValid() const { return is_valid_; }

const std::string& Token::GetName() const { return name_; }

const std::string& Token::GetLanguage() const { return language_; }

const std::string& Token::GetType() const { return type_; }

const std::string& Token::GetScope() const { return scope_; }

const std::string& Token::GetPath() const { return path_; }

const std::string& Token::GetAnchor() const { return anchor_; }

const std::string& Token::GetDeclaredIn() const { return declared_in_; }

std::string Token::GetIndexName() const { return name_; }

std::string Token::GetIndexType() const { return type_; }

std::string Token::GetIndexPath() const { return path_; }

}  // namespace d2d
