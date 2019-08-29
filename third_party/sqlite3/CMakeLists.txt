add_library(sqlite3
  "sqlite3.h"
  "sqlite3.c"
)

target_include_directories(sqlite3
  PUBLIC
    "."
)
