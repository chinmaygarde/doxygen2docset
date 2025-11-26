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

find_package(unofficial-gumbo CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

target_link_libraries(doxygen2docset_lib
  PUBLIC
    tinyxml2::tinyxml2
    unofficial::sqlite3::sqlite3
    unofficial::gumbo::gumbo
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
