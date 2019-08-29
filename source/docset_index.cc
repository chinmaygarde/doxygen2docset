#include "docset_index.h"

#include <stdio.h>

#include <utility>

#include "logger.h"

namespace d2d {

static std::pair<bool, std::string> RunSingleStatement(
    sqlite3* db, const std::string& statement) {
  if (db == nullptr) {
    return {false, "Database is not open,"};
  }
  auto callback = [](void*, int, char**, char**) { return 0; };
  char* error = nullptr;
  auto result = sqlite3_exec(db, statement.c_str(), callback, nullptr, &error);
  if (result != SQLITE_OK) {
    std::string error_string(error);
    sqlite3_free(error);
    return {false, error_string};
  }
  return {true, ""};
}

DocsetIndex::DocsetIndex(const std::string& database_name) {
  if (database_name.size() == 0) {
    D2D_ERROR << "Database name was empty";
    return;
  }

  // Remove the database index if one already exists.
  ::remove(database_name.c_str());

  auto result = sqlite3_open(database_name.c_str(), &database_);
  if (result != SQLITE_OK) {
    D2D_ERROR << "Could not create database file " << database_name;
    return;
  }

  auto create_result = RunSingleStatement(
      database_,
      "CREATE TABLE searchIndex(id INTEGER PRIMARY KEY, name TEXT, type TEXT, "
      "path TEXT);");

  if (!create_result.first) {
    D2D_ERROR << "Could not create index table: " << create_result.second;
    return;
  }

  auto index_result = RunSingleStatement(
      database_,
      "CREATE UNIQUE INDEX anchor ON searchIndex(name, type, path);");

  if (!index_result.first) {
    D2D_ERROR << "Could not create the index: " << index_result.second;
    return;
  }

  {
    std::string token_statement_string =
        "INSERT OR IGNORE INTO searchIndex(name, type, path) VALUES (?, ?, ?);";
    auto res =
        sqlite3_prepare_v2(database_, token_statement_string.c_str(),
                           static_cast<int>(token_statement_string.size()),
                           &token_statement_, nullptr);
    if (res != SQLITE_OK) {
      D2D_ERROR << "Could not create insertion statement.";
      return;
    }
  }

  is_valid_ = true;
}

DocsetIndex::~DocsetIndex() {
  auto result = ::sqlite3_finalize(token_statement_);
  if (result != SQLITE_OK) {
    D2D_ERROR << "Could not finalize statement.";
  }
  ::sqlite3_close(database_);
}

bool DocsetIndex::IsValid() const { return is_valid_; }

bool DocsetIndex::AddTokens(const std::vector<Token>& tokens) {
  if (!is_valid_) {
    D2D_ERROR << "Could not add tokens to an invalid docset index.";
    return false;
  }

  auto begin_result = RunSingleStatement(database_, "BEGIN TRANSACTION;");
  if (!begin_result.first) {
    D2D_ERROR << "Could not begin the transaction.";
    return false;
  }

  for (const auto& token : tokens) {
    if (::sqlite3_reset(token_statement_) != SQLITE_OK) {
      D2D_ERROR << "Could not reset the statement.";
      return false;
    }

    if (::sqlite3_clear_bindings(token_statement_) != SQLITE_OK) {
      D2D_ERROR << "Could clear previous statement bindings.";
      return false;
    }

    const auto& name = token.GetName();
    const auto& type = token.GetType();
    const auto& path = token.GetPath();

    if (::sqlite3_bind_text(token_statement_, 1, name.data(),
                            static_cast<int>(name.size()),
                            SQLITE_TRANSIENT) != SQLITE_OK) {
      D2D_ERROR << "Could not bind name.";
      return false;
    }

    if (::sqlite3_bind_text(token_statement_, 2, type.data(),
                            static_cast<int>(type.size()),
                            SQLITE_TRANSIENT) != SQLITE_OK) {
      D2D_ERROR << "Could not bind type.";
      return false;
    }

    if (::sqlite3_bind_text(token_statement_, 3, path.data(),
                            static_cast<int>(path.size()),
                            SQLITE_TRANSIENT) != SQLITE_OK) {
      D2D_ERROR << "Could not bind path.";
      return false;
    }

    if (::sqlite3_step(token_statement_) != SQLITE_DONE) {
      D2D_ERROR << "Could not step on the statement.";
      return false;
    }
  }

  auto end_result = RunSingleStatement(database_, "END TRANSACTION;");
  if (!end_result.first) {
    D2D_ERROR << "Could not end the transaction.";
    return false;
  }

  return true;
}

}  // namespace d2d
