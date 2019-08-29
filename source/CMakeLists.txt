add_library(doxygen2docset
  STATIC
    "builder.cc"
    "builder.h"
    "docset_index.cc"
    "docset_index.h"
    "file.cc"
    "file.h"
    "logger.h"
    "macros.h"
    "plist_parser.cc"
    "plist_parser.h"
    "token.cc"
    "token.h"
    "token_parser.cc"
    "token_parser.h"
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
