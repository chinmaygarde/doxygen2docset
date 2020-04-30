# This source file is part of doxygen2docset.
# Licensed under the MIT License. See LICENSE.md file for details.

cmake_minimum_required(VERSION 3.7.2)

set(CMAKE_CXX_STANDARD 14)

project("doxygen2docset" LANGUAGES CXX C)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc ")
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build deps as a static library." FORCE)

add_subdirectory("third_party/googletest" EXCLUDE_FROM_ALL)
add_subdirectory("third_party/tinyxml2" EXCLUDE_FROM_ALL)
add_subdirectory("third_party/sqlite3" EXCLUDE_FROM_ALL)
add_subdirectory("third_party/gumbo" EXCLUDE_FROM_ALL)

add_subdirectory("source")
add_subdirectory("tests")

# Debian Packages on Linux.
if(UNIX AND NOT APPLE)
  set(CPACK_PACKAGE_NAME "doxygen2docset")
  set(CPACK_PACKAGE_VENDOR "Chinmay Garde")
  set(CPACK_PACKAGE_CONTACT "chinmaygarde@gmail.com")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Dash or Zeal Docsets from Doxygen Documentation")
  set(CPACK_PACKAGE_VERSION_MAJOR 0)
  set(CPACK_PACKAGE_VERSION_MINOR 1)
  set(CPACK_PACKAGE_VERSION_PATCH 1)
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
  set(CPACK_GENERATOR "DEB")
  include(CPack)
endif(UNIX AND NOT APPLE)
