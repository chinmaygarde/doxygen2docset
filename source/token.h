// This source file is part of doxygen2docset.
// Licensed under the MIT License. See LICENSE.md file for details.

#pragma once

#include <tinyxml2.h>

#include <map>
#include <string>
#include <vector>

#include "macros.h"

namespace d2d {

class Token {
 public:
  Token();

  Token(const tinyxml2::XMLElement* element);

  Token(Token&&);

  Token(const Token& token);

  Token& operator=(const Token&);

  ~Token();

  bool IsValid() const;

  std::string GetIndexName() const;

  std::string GetIndexType() const;

  std::string GetIndexPath() const;

  const std::string& GetName() const;

  const std::string& GetLanguage() const;

  const std::string& GetType() const;

  const std::string& GetScope() const;

  const std::string& GetPath() const;

  const std::string& GetAnchor() const;

  const std::string& GetDeclaredIn() const;

  using TOCLinks = std::map<std::string, std::vector<Token>>;
  static TOCLinks GetTokensByFile(const std::vector<Token>& tokens);

 private:
  bool is_valid_ = false;
  std::string name_;
  std::string language_;
  std::string type_;
  std::string scope_;
  std::string path_;
  std::string anchor_;
  std::string declared_in_;
};

}  // namespace d2d
