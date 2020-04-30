# This source file is part of doxygen2docset.
# Licensed under the MIT License. See LICENSE.md file for details.

add_library(doxygen2docset_lib
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
    "html_parser.h"
    "html_parser.cc"
)

target_link_libraries(doxygen2docset_lib
  PUBLIC
    tinyxml2
    sqlite3
    gumbo
)

target_include_directories(doxygen2docset_lib
  PUBLIC
    "."
)

add_executable(doxygen2docset "main.cc")

target_link_libraries(doxygen2docset
    PRIVATE
        doxygen2docset_lib
)

set_target_properties(doxygen2docset PROPERTIES LINK_FLAGS_RELEASE -s)

install(TARGETS doxygen2docset
        RUNTIME
        DESTINATION bin)
