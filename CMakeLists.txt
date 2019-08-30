cmake_minimum_required(VERSION 3.8.2)

set(CMAKE_CXX_STANDARD 17)

project("doxygen2docset" LANGUAGES CXX C)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_EXE_LINKER_FLAGS " ${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc ")
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build deps as a static library." FORCE)
add_subdirectory("third_party/googletest")
add_subdirectory("third_party/tinyxml2")
add_subdirectory("third_party/sqlite3")

add_subdirectory("source")
add_subdirectory("tests")
