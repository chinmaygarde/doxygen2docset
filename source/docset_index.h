// This source file is part of doxygen2docset.
// Licensed under the MIT License. See LICENSE.md file for details.

#pragma once

#include <sqlite3.h>

#include <string>
#include <vector>

#include "macros.h"
#include "token.h"

namespace d2d {

class DocsetIndex {
 public:
  DocsetIndex(const std::string& database_name);

  ~DocsetIndex();

  bool IsValid() const;

  bool AddTokens(const std::vector<Token>& tokens);

 private:
  sqlite3* database_ = nullptr;
  sqlite3_stmt* token_statement_ = nullptr;
  bool is_valid_ = false;

  D2D_DISALLOW_COPY_AND_ASSIGN(DocsetIndex);
};

}  // namespace d2d
