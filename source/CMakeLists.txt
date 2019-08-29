add_library(doxygen2docset
  "definition_parser.h"
  "definition_parser.cc"
  "logger.h"
  "macros.h"
)

target_link_libraries(doxygen2docset
  PUBLIC
    tinyxml2
)

target_include_directories(doxygen2docset
  PUBLIC
    "."
)
