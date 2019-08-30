get_filename_component(FIXTURES_DIRECTORY fixtures ABSOLUTE)

set(D2D_FIXTURES_LOCATION ${FIXTURES_DIRECTORY})

configure_file(fixture.h.in fixture.h @ONLY)

add_executable(doxygen2docset_unittests
  "test_main.cc"
  "doxygen2docset_unittests.cc"
)

target_include_directories(doxygen2docset_unittests
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(doxygen2docset_unittests
  PRIVATE
    gtest
    doxygen2docset_lib
)
