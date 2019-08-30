add_library(sqlite3
  STATIC
    "sqlite3.h"
    "sqlite3.c"
)

target_compile_definitions(sqlite3
  PRIVATE
    SQLITE_OMIT_LOAD_EXTENSION
    SQLITE_THREADSAFE=0
)

target_include_directories(sqlite3
  PUBLIC
    "."
)
