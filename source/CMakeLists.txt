add_library(doxygen2docset
  "token_parser.h"
  "token_parser.cc"
  "logger.h"
  "macros.h"
  "token.h"
  "token.cc"
  "docset_index.cc"
  "docset_index.h"
)

target_link_libraries(doxygen2docset
  PUBLIC
    tinyxml2
    sqlite3
)

target_include_directories(doxygen2docset
  PUBLIC
    "."
)
