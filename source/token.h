#pragma once

#include <tinyxml2.h>
#include <string>

#include "macros.h"

namespace d2d {

class Token {
 public:
  Token(const tinyxml2::XMLElement* element);

  Token(Token&&);

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

 private:
  bool is_valid_ = false;
  std::string name_;
  std::string language_;
  std::string type_;
  std::string scope_;
  std::string path_;
  std::string anchor_;
  std::string declared_in_;

  D2D_DISALLOW_COPY_AND_ASSIGN(Token);
};

}  // namespace d2d
