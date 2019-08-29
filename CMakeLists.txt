cmake_minimum_required(VERSION 3.12.2)

set(CMAKE_CXX_STANDARD 17)

project("doxygen2docset" LANGUAGES CXX C)

add_subdirectory("third_party/googletest")
add_subdirectory("third_party/tinyxml2")

add_subdirectory("source")
add_subdirectory("tests")
